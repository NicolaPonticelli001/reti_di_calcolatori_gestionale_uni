#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include <iostream>
#include <string.h>

using namespace std;

int main(){
    cout<<"Benvenuto nel gestionale dell'universita'"<<endl;
    int scelta;
    do{
        cout<<"Seleziona cosa vuoi fare"<<endl;
        cout<<"1)Aggiungi un nuovo appello"<<endl;
        cout<<"2)Aggiungi un nuovo esame"<<endl;
        cout<<"0)Esci"<<endl;

        cin>>scelta;
        cout<<endl;
        switch(scelta){
            case 1:{
                struct Packet richiesta,risposta;
                struct Appello appello;
                string IP="127.0.0.1",data,ora;
                ClientSocket client=ClientSocket(IP,UNI_SERVER_PORT);   //Creazione dell'oggetto client per la comunicazione
                client.clientSetup();   //Inizializzazione dati per la comunicazione
                
                //Raccolta dati appello da inserire
                cout<<"Inserisci il codice dell'appello: ";
                cin>>appello.codiceAppello;
                cout<<"Inserisci il codice dell'esame: ";
                cin>>appello.codiceEsame;
                cout<<"Inserisci la data formato 'GG-MM-AAAA': ";
                cin>>data;
                cout<<"Inserisci l'ora in formato 'hh:mm': ";
                cin>>ora;
                data+=" "+ora;
                strcpy(appello.data,data.c_str());
                appello.data[16] = '\0';

                richiesta.request=INS_APPELLO;  //Scelta dell'operazione da fare

                client.Connect();   //Connessione al server

                client.Write(&richiesta,sizeof(richiesta)); //Invio richiesta al server
                client.Write(&appello,sizeof(appello)); //Invio dati appello da inserire

                client.Read(&risposta,sizeof(risposta)); //Attesa e ricezione della risposta
                if(risposta.error.getCode()==OK){
                    cout<<"Appello inserito con successo"<<endl;
                }
                else{
                    cout<<"Errore nell'inserimento, riprova"<<endl;
                }
                client.disconnect();
                break;
            }
            case 2:{
                struct Packet richiesta,risposta;
                struct Esame esame;
                bool tipo_modalita=true;
                string descrizione,ora;
                string IP="127.0.0.1";
                ClientSocket client = ClientSocket(IP,UNI_SERVER_PORT);   //Creazione dell'oggetto client per la comunicazione
                client.clientSetup();   //Inizializzazione dati per la comunicazione
                
                //Raccolta dati esame da inserire
                cout<<"Inserisci il codice dell'esame: ";
                cin>>esame.codiceEsame;
                cout<<"Inserisci il corso a cui è associato l'esame: ";
                cin>>esame.codiceCorso;
                cout<<"Inserisci il tipo di esame ('prova' -> 0,'esame' -> 1): ";
                cin>>tipo_modalita;
                if(tipo_modalita){
                    strcpy(esame.tipo,"Esame");
                }
                else{
                    strcpy(esame.tipo,"Prova");
                }
                esame.tipo[5]='\0';
                cout<<"Inserisci la modalita' dell'esame ('scritto' -> 0,'orale' -> 1): ";
                cin>>tipo_modalita;
                if(tipo_modalita){
                    strcpy(esame.modalita,"Orale");
                    esame.modalita[5]='\0';
                }
                else{
                    strcpy(esame.modalita,"Scritto");
                    esame.modalita[7]='\0';
                }
                cout<<"Inserisci la descrizione (default: vuota): ";
                getline(cin, descrizione);  // Pulizia buffer di input
                getline(cin, descrizione);
                if(descrizione.length()-1==0){
                    esame.descrizione[0] = '\0';
                }
                else{
                    strcpy(esame.descrizione,descrizione.c_str());
                    esame.descrizione[descrizione.length()]='\0';
                }

                richiesta.request=INS_ESAME;    //Impostazione del tipo di richiesta

                client.Connect();   //Connessione al server

                client.Write(&richiesta,sizeof(richiesta)); //Invio richiesta al server
                client.Write(&esame,sizeof(esame)); //Invio dati dell'esame da inserire

                client.Read(&risposta,sizeof(risposta));    //Attesa della risposta
                if(risposta.error.getCode()==OK){
                    cout<<"Esame inserito con successo"<<endl;
                }
                else{
                    cout<<"Errore nell'inserimento"<<endl;
                }

                client.disconnect();
                break;
            }
            default:
                cout<<"Scelta non riconosciuta, riprova"<<endl;
                break;
        }
        cout<<endl;
    }while(scelta!=0);
    cout<<"Uscita..."<<endl;

    return 0;
}