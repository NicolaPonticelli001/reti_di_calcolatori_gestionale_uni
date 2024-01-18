#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(){
    string ip="127.0.0.1";
    ClientSocket client=ClientSocket(ip,SEGRETERIA_SERVER_PORT);
    int matricola,tentativi=0;
    bool authenticated=false;

    //Procedura di login obbligatoria
    cout<<"Benvenuto nel gestionale dell'universtita'"<<endl;
    cout<<"Effettua l'accesso con la tua matricola"<<endl;
    do{
        cin>>matricola;

        struct Packet pacchetto;
        //pacchetto.request=LOGIN;
        pacchetto.data[MATRICOLA_STUDENTE]=matricola;

        client.clientSetup();

        client.Connect();   //Connessione al server per l'autenticazione

        client.Write((const void*)&pacchetto,sizeof(pacchetto));   //Invio richiesta al server

        client.Read(&pacchetto,sizeof(pacchetto));    //Attesa

        if(pacchetto.error.getCode()==OK)
            authenticated=true;
        else{
            cout<<"Errore di autenticazione"<<endl;
            cout<<"Matricola non riconosciuta"<<endl;
            cout<<"Riprova l'accesso"<<endl;
            tentativi++; //superati 3 tentativi di accesso, il programma termina
        }
        
    }while((tentativi<3)&&(!authenticated));
    
    if(!authenticated){
        cout<<"Numero di tentetivi ecceduto, uscita dal programma"<<endl;
        return 1;
    }

    int scelta;
    cout<<"Ben tornato!"<<endl;
    cout<<"Scegli cosa fare"<<endl;
    do{
        cout<<"1)Visualizza gli appelli disponibili"<<endl;
        cout<<"2)Prenotati ad un appello"<<endl;
        cout<<"3)Visualizza gli appelli a cui prenotato"<<endl;
        cout<<"0)Esci"<<endl;
        cin>>scelta;
        switch(scelta){
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 0:
                cout<<"Uscita..."<<endl;
                break;
            default:
                cout<<"Scelta non riconosciuta, riprova"<<endl;
                break;
        }
    }while(scelta!=0);
    
    cout<<"Uscita dal programma..."<<endl;

    return 0;
}