#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include <iostream>
#include <string.h>

using namespace std;

int main(){
    string IP="127.0.0.1";
    ClientSocket client=ClientSocket(IP,SEGRETERIA_SERVER_PORT);

    cout<<"Benvenuto nel gestionale dell'universita'"<<endl;
    int scelta;
    do{
        cout<<"Seleziona cosa vuoi fare"<<endl;
        cout<<"1)Aggiungi un nuovo appello"<<endl;
        cout<<"0)Esci"<<endl;
        cin>>scelta;
        if(scelta==1){
            struct Packet richiesta,risposta;
            struct Appello appello;
            string IP="127.0.0.1",data,ora;
            ClientSocket client=ClientSocket(IP,UNI_SERVER_PORT);
            client.clientSetup();
            
            cout<<"Inserisci il codice dell'appello: ";
            cin>>appello.codiceAppello;
            cout<<"Inserisci il codice dell'esame: ";
            cin>>appello.codiceEsame;
            cout<<"Inserisci la data formato 'GG-MM-AAAA':";
            cin>>data;
            cout<<"Inserisci l'ora in formato 'hh:mm': ";
            cin>>ora;
            data+=" "+ora;
            strcpy(appello.data,data.c_str());

            richiesta.request=INS_APPELLO;

            client.Connect();

            client.Write(&richiesta,sizeof(richiesta)); //Invio richiesta al server
            client.Write(&appello,sizeof(appello)); //Invio dati appello da inserire

            client.Read(&risposta,sizeof(risposta)); //Attesa della risposta

            if(risposta.error.getCode()==OK){
                cout<<"Appello inserito con successo"<<endl;
            }
            else{
                cout<<"Errore nell'inserimento, riprova"<<endl;
            }
        }
        else{
            if(scelta!=0){
                cout<<"Scelta non riconosciuta"<<endl;
            }
        }
    }while(scelta!=0);
    cout<<"Uscita..."<<endl;

    return 0;
}