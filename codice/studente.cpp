#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include <iostream>
#include <string>

using namespace std;

void printAppelli(AppelloDisponibile *appelli,int num_righe);
void printAppelli(AppelloPrenotato *appelli,int num_righe);
void printEsami(EsameDisponibile *esami,int num_righe);
void printCorsi(Corsi *corsi,int num_righe);

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
        cout<<endl;

        struct Packet pacchetto;
        pacchetto.data[MATRICOLA_STUDENTE]=matricola;
        
        client.clientSetup();

        client.Connect();   //Connessione al server per l'autenticazione

        client.Write((const void*)&pacchetto,sizeof(pacchetto));   //Invio richiesta al server

        client.Read((void *)&pacchetto,sizeof(pacchetto));    //Attesa e ricezione

        client.disconnect();

        if(pacchetto.error.getCode()==OK)
            authenticated=true;
        else{
            tentativi++; //superati 3 tentativi di accesso, il programma termina
            cout<<"Errore di autenticazione"<<endl;
            cout<<"Matricola non riconosciuta"<<endl;
            cout<<"Riprova l'accesso"<<endl;
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
        cout<<"1)Visualizza gli appelli disponibili per un esame scelto"<<endl;
        cout<<"2)Prenotati ad un appello"<<endl;
        cout<<"3)Visualizza gli appelli a cui sei prenotato"<<endl;
        cout<<"0)Esci"<<endl;
        cin>>scelta;
        switch(scelta){
            case 1:{
                Packet richiesta,risposta;
                int corso,esame;
                client.Connect();   //Connessione al server

                richiesta.request=VIEW_CORSI;
                client.Write(&richiesta,sizeof(richiesta)); //Invio richiesta al server
                client.Read(&risposta,sizeof(risposta));
                if(risposta.error.getCode()==OK){
                    int num_righe=risposta.data[RIGHE_QUERY];
                    cout << "Righe: " << num_righe << endl;
                    Corsi *corsi=new Corsi[num_righe];
                    client.Read(corsi,sizeof(EsameDisponibile)*num_righe);
                    cout<<"Appelli disponibili per l'esame "<<corso<<":"<<endl;
                    printCorsi(corsi,num_righe);
                    delete[] corsi;
                }
                else{
                    cout<<"Errore nel caricamento dei corsi"<<endl;
                    break;
                }

                /*
                1)Selezionare il corso
                2)Ottenimento degli esami
                3)Selezionare l'esame
                4)Ottenimento gli appelli per quell'esame
                */

                //1)Selezionare il corso
                cout<<"Inserisci il codice del corso: ";
                cin>>corso;
                //Riempimento dei campi del pacchetto di richiesta al fine di poterla gestire
                richiesta.request=VIEW_ESAMI;
                richiesta.data[MATRICOLA_STUDENTE]=matricola;
                richiesta.data[CORSO]=corso;
                client.Write(&richiesta,sizeof(richiesta)); //Invio richiesta al server
                client.Read(&risposta,sizeof(risposta));
                if(risposta.error.getCode()==OK){    //Nessun errore
                    //2)Ottenimento esami
                    int num_righe=risposta.data[RIGHE_QUERY];
                    if(num_righe!=0){   //Ci sono esami disponibili per il corso scelto
                        cout << "Righe: " << num_righe << endl;
                        EsameDisponibile *esami=new EsameDisponibile[num_righe];
                        client.Read(esami,sizeof(EsameDisponibile)*num_righe);
                        cout<<"Appelli disponibili per l'esame "<<corso<<":"<<endl;
                        printEsami(esami,num_righe);
                        delete[] esami;

                        //3)Selezionare l'esame
                        cout<<"Inserisci l'esame di cui vuoi vedere gli appelli"<<endl;
                        cin>>esame;
                        richiesta.request=VIEW_APP;
                        richiesta.data[MATRICOLA_STUDENTE]=matricola;
                        richiesta.data[ESAME]=esame;
                        client.Write(&richiesta,sizeof(richiesta));
                        client.Read(&risposta,sizeof(risposta));
                        if(risposta.error.getCode()==OK){
                            
                            //4)Ottenimento degli appelli per l'esame scelto
                            num_righe=risposta.data[RIGHE_QUERY];
                            if(num_righe!=0){  //Ci sono appelli disponibili per l'esame scelto
                                cout << "Righe: " << num_righe << endl;
                                AppelloDisponibile *appelli=new AppelloDisponibile[num_righe];
                                client.Read(appelli,sizeof(AppelloDisponibile)*num_righe);
                                cout<<"Appelli disponibili:"<<endl;
                                printAppelli(appelli,num_righe);
                                delete[] appelli;
                            }
                            else{
                                cout<<"Nessun appello disponibile per il corso scelto"<<endl;
                            }
                        }
                        else{
                            cout<<"Errore nel caricamento degli appelli"<<endl;
                        }
                    }
                    else{
                        cout<<"Nessun esame disponibile per il corso scelto"<<endl;
                    }
                }
                else{
                    cout<<"Errore nel caricamento degli esami"<<endl;
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
                cout<<"Richiesta PREN_STUD inviata"<<endl;
                client.Write(&richiesta,sizeof(richiesta));
                client.Read(&risposta,sizeof(risposta));
                if(risposta.error.getCode()==OK){
                    cout<<"Prenotazione effettuata!"<<endl;
                    cout<<"Sei il numero: "<<risposta.data[GENERIC_DATA]<<endl;
                }
                else{
                    cout<<"Errore di prenotazione"<<endl;
                }
                break;
            }
            case 3:{
                Packet richiesta,risposta;
                richiesta.request=VIEW_APP_P;
                richiesta.data[MATRICOLA_STUDENTE]=matricola;
                client.clientSetup();
                client.Connect();
                client.Write(&richiesta,sizeof(richiesta));
                client.Read(&risposta,sizeof(risposta));
                if(risposta.error.getCode()==OK){
                    //Elaborazione appelli
                    int num_righe=risposta.data[RIGHE_QUERY];
                    cout << "Righe: " << num_righe << endl;
                    AppelloPrenotato *appelli=new AppelloPrenotato[num_righe];
                    client.Read(appelli,sizeof(AppelloPrenotato)*num_righe);
                    cout<<"Appelli disponibili:"<<endl;
                    printAppelli(appelli,num_righe);
                    delete[] appelli;
                }
                else{
                    cout<<"Errore nel caricamento degli appelli"<<endl;
                }
                cout<<endl;
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
        client.disconnect();
        cout<<endl;
    }while(scelta!=0);
    
    cout<<"Uscita dal programma..."<<endl;

    return 0;
}

void printAppelli(AppelloDisponibile *appelli,int num_righe){
    for(int i=0;i<num_righe;i++){
        cout<<"Appello: "<<appelli[i].codiceAppello<<endl;
        cout<<"Nome appello: "<<appelli[i].nome<<endl;
        cout<<"Data appello: "<<appelli[i].data<<endl;
        cout<<endl;
    }
}

void printAppelli(AppelloPrenotato *appelli,int num_righe){
    for(int i=0;i<num_righe;i++){
        cout<<"Appello: "<<appelli[i].codice<<endl;
        cout<<"Data: "<<appelli[i].data<<endl;
        cout<<"Nome: "<<appelli[i].nome<<endl;
        cout<<"Tipo: "<<appelli[i].tipo<<endl;
        cout<<"Modalita': "<<appelli[i].modalita<<endl;
        cout<<"Descrizione: "<<appelli[i].descrizione<<endl;
        cout<<"#Prenotazione: "<<appelli[i].numeroPrenotazione<<endl;
        cout<<endl;
    }
}