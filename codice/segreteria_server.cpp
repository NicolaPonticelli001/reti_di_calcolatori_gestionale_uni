#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include <iostream>
#include <unistd.h>

using namespace std;

int tStudenti_prova[5]={1,2,3,4,5};

int main(){
    pid_t pid;
    ServerSocket server=ServerSocket(SEGRETERIA_SERVER_PORT);
    
    server.serverSetup();

    while(1){
        server.Accept_NoClient();

        pid=fork();
        if(pid==0){
            //Chiudi fd di ascolto
            server.closeListening();
            
            struct Packet richiesta;
            server.Read((void *)&richiesta,sizeof(richiesta));
            
            switch(richiesta.request){
                case LOGIN:{
                    cout<<"Richiesta di autenticazione studente"<<endl;
                    cout<<"Autenticazione in corso..."<<endl;

                    //Query al DB

                    //Simulazione della query del DB
                    int i=0;
                    bool trovato=false;
                    while((!trovato)&&(i<5)){
                        if(tStudenti_prova[i]==richiesta.data[MATRICOLA_STUDENTE]){
                            trovato=true;
                        }
                        i++;
                    }
                    if(trovato){   //Studente trovato, autenticato
                        cout<<"Studente autenticato!"<<endl;
                        richiesta.error.setCode(OK);
                    }
                    else{   //Studente non trovato, non autenticato
                        cout<<"Errore di autenticazione, studente non autenticato!"<<endl;
                        richiesta.error.setCode(AUTH_ERROR);
                    }
                    //Fine simulazione

                    //Invio esito
                    server.Write(&richiesta,sizeof(richiesta));
                    break;
                }
                case PREN_STUD:{
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    Packet richiesta_prentoazione=richiesta,risposta_prenotazione,risposta;
                    client.Connect();
                    client.Write(&richiesta_prentoazione,sizeof(richiesta_prentoazione));
                    client.Read(&risposta_prenotazione,sizeof(richiesta_prentoazione));
                    client.disconnect();
                    if(risposta_prenotazione.error.getCode()==OK){
                        cout<<"Prenotazione effettuata con successo"<<endl;
                        cout<<"Studente "<<richiesta_prentoazione.data[MATRICOLA_STUDENTE]<<"-#"<<richiesta_prentoazione.data[GENERIC_DATA];
                        risposta=risposta_prenotazione;
                        server.Write(&risposta,sizeof(risposta));
                    }
                    break;
                }
                case VIEW_APP:{
                    Packet risposta_server;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta)); //Inoltro della richiesta al server universitario
                    client.Read(&risposta_server,sizeof(risposta_server));  //Attesa del risultato
                    if(risposta_server.error.getCode()==OK){
                        //int num_righe=risposta.request[RIGHE_QUERY]
                        //AppelloDisponibile *appelli=new AppelloDisponibile[num_righe];
                        //client.Read(appelli,sizeof(appelli)*num_righe);
                        //server.Write(appelli,sizeof(appelli)*num_righe);
                    }
                    else{
                        risposta_server.error.setCode(GENERIC);
                        server.Write(&risposta_server,sizeof(risposta_server));
                    }
                    client.disconnect();
                    break;
                }
                case VIEW_APP_P:{
                    Packet risposta_server;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta)); //Inoltro della richiesta al server universitario
                    client.Read(&risposta_server,sizeof(risposta_server));  //Attesa del risultato
                    if(risposta_server.error.getCode()==OK){
                        //int num_righe=risposta.request[RIGHE_QUERY]
                        //AppelloDisponibile *appelli=new AppelloDisponibile[num_righe];
                        //client.Read(appelli,sizeof(appelli)*num_righe);
                        //server.Write(appelli,sizeof(appelli)*num_righe);
                        //delete [appelli]
                    }
                    else{
                        risposta_server.error.setCode(GENERIC);
                        server.Write(&risposta_server,sizeof(risposta_server));
                    }
                    client.disconnect();
                    break;
                }
                default:{

                    break;
                }
            }
            server.closeServer();
            exit(0);
        }
    }


    return 0;
}