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
                    Packet richiesta_autenticazione=richiesta;
                    cout<<"Richiesta di autenticazione studente"<<endl;
                    cout<<"Autenticazione in corso..."<<endl;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    client.Connect();
                    //Richiesta Query al DB
                    client.Write(&richiesta_autenticazione,sizeof(richiesta_autenticazione));
                    client.Read(&richiesta_autenticazione,sizeof(richiesta_autenticazione));
                    client.disconnect();
                    if(richiesta_autenticazione.error.getCode()==OK){
                        cout<<"Studente autenticato!"<<endl;
                    }
                    else{
                        cout<<"Errore di autenticazione, studente non autenticato!"<<endl;
                        richiesta_autenticazione.error.setCode(AUTH_ERROR);
                    }
                    //Invio esito
                    server.Write(&richiesta_autenticazione,sizeof(richiesta));
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
                        cout<<"Studente "<<richiesta_prentoazione.data[MATRICOLA_STUDENTE]<<"-#"<<risposta_prenotazione.data[GENERIC_DATA];
                        risposta=risposta_prenotazione;
                        server.Write(&risposta,sizeof(risposta));
                    }
                    break;
                }
                case VIEW_APP:{
                    cout << "Dentro VIEW_APP. Matricola: " << richiesta.data[MATRICOLA_STUDENTE] << endl;
                    Packet risposta_server;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta)); //Inoltro della richiesta al server universitario
                    client.Read(&risposta_server,sizeof(risposta_server));  //Attesa del risultato
                    if(risposta_server.error.getCode()==OK){
                        int num_righe=risposta_server.data[RIGHE_QUERY];
                        AppelloDisponibile *appelli=new AppelloDisponibile[num_righe];
                        server.Write(&risposta_server, sizeof(risposta_server));
                        client.Read(appelli,sizeof(AppelloDisponibile)*num_righe);
                        server.Write(appelli,sizeof(AppelloDisponibile)*num_righe);
                        delete[] appelli;
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
                        int num_righe=risposta_server.data[RIGHE_QUERY];
                        AppelloPrenotato *appelli=new AppelloPrenotato[num_righe];
                        server.Write(&risposta_server, sizeof(risposta_server));
                        client.Read(appelli,sizeof(AppelloPrenotato)*num_righe);
                        server.Write(appelli,sizeof(AppelloPrenotato)*num_righe);
                        delete[] appelli;
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