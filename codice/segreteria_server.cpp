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
                    break;
                    
                }
                case VIEW_APP:{

                    break;
                }
                case VIEW_APP_P:{

                    break;
                }
                default:{

                    break;
                }
            }
            
            exit(0);
        }
    }


    return 0;
}