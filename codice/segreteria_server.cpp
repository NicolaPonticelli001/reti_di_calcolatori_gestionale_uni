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
            server.closeListening();    //Chiusura fd di ascolto
            
            struct Packet richiesta;
            server.Read((void *)&richiesta,sizeof(richiesta));
            cout<<"Ricevuta la richiesta: "<<richiesta.request<<endl;
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
                    Packet risposta;
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta));
                    client.Read((void*)&risposta,sizeof(risposta));
                    client.disconnect();
                    if(risposta.error.getCode()==OK){
                        cout<<"Prenotazione effettuata con successo"<<endl;
                        cout<<"Studente "<<richiesta.data[MATRICOLA_STUDENTE]<<"-#"<<risposta.data[GENERIC_DATA]<<endl;
                    }
                    server.Write(&risposta,sizeof(risposta));
                    break;
                }
                case VIEW_CORSI:{
                    cout<<"Richiesta visione corsi disponibili"<<endl;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    Packet risposta_server;
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta));
                    client.Read(&risposta_server,sizeof(risposta_server));
                    if(risposta_server.error.getCode()==OK){
                        int num_righe=risposta_server.data[RIGHE_QUERY];
                        cout<<"Numero corsi trovati: "<<num_righe<<endl;
                        server.Write(&risposta_server,sizeof(risposta_server));
                        if(num_righe!=0){
                            Corso *corsi = new Corso[num_righe];
                            cout << "Prima di read" << endl;
                            client.Read(corsi,sizeof(Corso)*num_righe);
                            cout << "Dopo read" << endl;
                            server.Write(corsi,sizeof(Corso)*num_righe);
                            delete[] corsi;
                        }
                    }
                    else{
                        risposta_server.error.setCode(GENERIC);
                        server.Write(&risposta_server,sizeof(risposta_server));
                    }
                    client.disconnect();
                    break;
                }
                case VIEW_ESAMI:{
                    cout<<"Richiesta visione esami per il corso: "<<richiesta.data[CORSO]<<""<<endl;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    Packet risposta_server;
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta));
                    client.Read(&risposta_server,sizeof(risposta_server));
                    if(risposta_server.error.getCode()==OK){
                        int num_righe=risposta_server.data[RIGHE_QUERY];
                        cout<<"Numero corsi trovati: "<<num_righe<<endl;
                        server.Write(&risposta_server,sizeof(risposta_server));
                        if(num_righe!=0){
                            Esame *esami = new Esame[num_righe];
                            client.Read(esami,sizeof(Esame)*num_righe);
                            server.Write(esami,sizeof(Esame)*num_righe);
                            delete[] esami;
                        }
                    }
                    else{
                        risposta_server.error.setCode(GENERIC);
                        server.Write(&risposta_server,sizeof(risposta_server));
                    }
                    client.disconnect();
                    break;
                }
                case VIEW_APP:{
                    cout<<"Richiesta visione appelli disponibili, studente: " << richiesta.data[MATRICOLA_STUDENTE]<<endl;
                    Packet risposta_server;
                    ClientSocket client=ClientSocket("127.0.0.1",UNI_SERVER_PORT);
                    client.clientSetup();
                    client.Connect();
                    client.Write(&richiesta,sizeof(richiesta)); //Inoltro della richiesta al server universitario
                    client.Read(&risposta_server,sizeof(risposta_server));  //Attesa del risultato
                    if(risposta_server.error.getCode()==OK){
                        //Gestione appelli
                        int num_righe=risposta_server.data[RIGHE_QUERY];
                        cout<<"Numero appelli trovati per lo studente "<<risposta_server.data[MATRICOLA_STUDENTE]<<": "<<risposta_server.data[RIGHE_QUERY]<<endl;
                        server.Write(&risposta_server, sizeof(risposta_server));    //Invio allo studente il numero di appelli trovati
                        if(num_righe!=0){ 
                            AppelloDisponibile *appelli=new AppelloDisponibile[num_righe];
                            client.Read(appelli,sizeof(AppelloDisponibile)*num_righe);  //Ottenimento risultato della query dal server universitario
                            server.Write(appelli,sizeof(AppelloDisponibile)*num_righe); //Invio risultato della query allo studente
                            delete[] appelli;
                        }
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
                        cout<<"Numero appelli trovati per lo studente "<<risposta_server.data[MATRICOLA_STUDENTE]<<": "<<risposta_server.data[RIGHE_QUERY]<<endl;
                        server.Write(&risposta_server, sizeof(risposta_server));
                        if(num_righe!=0){
                            AppelloPrenotato *appelli=new AppelloPrenotato[num_righe];
                            client.Read(appelli,sizeof(AppelloPrenotato)*num_righe);
                            server.Write(appelli,sizeof(AppelloPrenotato)*num_righe);
                            delete[] appelli;
                        }
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
            cout<<endl;
            exit(0);
        }
    }


    return 0;
}