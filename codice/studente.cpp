#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include <iostream>
#include <string>

using namespace std;

void printAppelli(AppelloDisponibile *appelli,int num_righe);
void printAppelli(AppelloPrenotato *appelli,int num_righe);

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

        client.Read((void *)&pacchetto,sizeof(pacchetto));    //Attesa

        client.disconnect();

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
        cout<<"3)Visualizza gli appelli a cui sei prenotato"<<endl;
        cout<<"0)Esci"<<endl;
        cin>>scelta;
        switch(scelta){
            case 1:{
                Packet richiesta;
                richiesta.request=VIEW_APP;
                richiesta.data[MATRICOLA_STUDENTE]=matricola;
                client.clientSetup();
                client.Write(&richiesta,sizeof(richiesta));
                client.Read(&risposta,sizeof(risposta));
                client.disconnect();
                if(risposta.error.getCode()==OK){
                    //Elaborazione appelli
                    //int num_righe=risposta.request[RIGHE_QUERY]
                    //AppelloDisponibile *appelli=new AppelloDisponibile[num_righe];
                    //client.Read(appelli,sizeof(appelli)*num_righe);
                    cout<<"Appelli disponibili:"<<endl;
                    //printAppelli(appelli,num_righe);
                    //delete [appelli];
                }
                else{
                    cout<<"Errore nel caricamento degli appelli"<<endl;
                }
                break;
            }
            case 2:{
                int appello;
                Packet richiesta,risposta;
                richiesta.request=PREN_STUD;
                cout<<"Inserisci il codice dell'appello a cui vuoi prenotarti: ";
                cin>>appello;
                richiesta.data[MATRICOLA_STUDENTE]=matricola;
                richiesta.data[APPELLO]=appello;
                client.clientSetup();
                client.Connect();
                client.Write(&richiesta,sizeof(richiesta));
                client.Read(&risposta,sizeof(risposta));
                if(risposta.error.getCode==OK){
                    cout<<"Prenotazione effettuata!"<<endl;
                    cout<<"Sei il numero: "<<risposta.data[GENERIC_DATA]<<endl;
                }
                else{
                    cout<<"Errore di prenotazione"<<endl;
                }
                client.disconnect();
                break;
            }
            case 3:{
                Packet richiesta;
                richiesta.request=VIEW_APP_P;
                richiesta.data[MATRICOLA_STUDENTE]=matricola;
                client.clientSetup();
                client.Write(&richiesta,sizeof(richiesta));
                client.Read(&risposta,sizeof(risposta));
                client.disconnect();
                if(risposta.error.getCode()==OK){
                    //Elaborazione appelli
                    //int num_righe=risposta.request[RIGHE_QUERY]
                    //AppelloPrenotato *appelli=new AppelloPrenotato[num_righe];
                    //client.Read(appelli,sizeof(appelli)*num_righe);
                    cout<<"Appelli disponibili:"<<endl;
                    //printAppelli(appelli,num_righe);
                    //delete [appelli];
                }
                else{
                    cout<<"Errore nel caricamento degli appelli"<<endl;
                }
                break;
            }
            case 0:{
                cout<<"Uscita..."<<endl;
                break;
            }
            default:{
                cout<<"Scelta non riconosciuta, riprova"<<endl;
                break;
            }
        }
    }while(scelta!=0);
    
    cout<<"Uscita dal programma..."<<endl;

    return 0;
}

void printAppelli(AppelloDisponibile *appelli,int num_righe){
    for(int i=0;i<num_righe;i++){
        cout<<"Appello:\t"<<appelli[i].codiceAppello<<endl;
        cout<<"Nome appello:\t"<<appelli[i].nome<<endl;
        cout<<"Data appello:\t"<<appelli[i].data<<endl;
    }
}

void printAppelli(AppelloPrenotato *appelli,int num_righe){
    for(int i=0;i<num_righe;i++){
        cout<<"Appello:\t"<<appelli[i].codice<<endl;
        cout<<"Data:\t"<<appelli[i].data<<endl;
        cout<<"Nome:\t"<<appelli[i].nome<<endl;
        cout<<"Tipo:\t"<<appelli[i].tipo<<endl;
        cout<<"Modalita'\t"<<appelli[i].modalita<<endl;
        cout<<"Descrizione\t"<<appelli[i].descrizione<<endl;
        cout<<"#Prenotazione"<<appelli[i].numeroPrenotazione<<endl;
        cout<<endl;
    }
}