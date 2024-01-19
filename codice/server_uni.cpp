#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <sqlite3.h>
#include "libs/include/Socket.hpp"
#include "libs/include/DataStructures.hpp"
#include "libs/include/DBMS.hpp"
#define DB_FILE_NAME "universita.db"    // Nome del file database

using namespace std;

// Converte il formato data da italiano a inglese (per salvare una data nel DB)
// oppure da inglese ad italiano (quando si ottiene una data dal DB)
string formatDate(string date);

void loginAction(struct Packet packet, ServerSocket server);
void insertAppelloAction(struct Packet packet, ServerSocket server);
void insertPrenotazioneAction(struct Packet packet, ServerSocket server);
void viewAppelli(struct Packet packet, ServerSocket server);
void viewAppelliPrenotati(struct Packet packet, ServerSocket server);

int main() {
    pid_t pid;
    ServerSocket server = ServerSocket(UNI_SERVER_PORT);
    struct Packet packet;

    Error error;
    
    server.serverSetup();

    while(1) {
        server.Accept_NoClient();
        cout << endl;
        pid = fork();

        if (pid < 0) {
            packet.error.setAll(GENERIC, "Fork", "Error while creating child process (pid < 0)");
            error.printError();
            server.Write((void*)&packet, sizeof(packet));
        } else if (pid == 0) {
            server.Read((void*)&packet, sizeof(packet));
            cout << "Pacchetto ricevuto. Codice richiesta: " << packet.request << endl << endl;

            // enum RequestType -> LOGIN = 0, INS_APPELLO = 1, PREN_STUD = 2, VIEW_APP = 3, VIEW_APP_P = 4
            switch(packet.request) {
                case LOGIN:
                    loginAction(packet, server);
                    break;
                
                case INS_APPELLO:
                    insertAppelloAction(packet, server);
                    break;
                
                case PREN_STUD:
                    insertPrenotazioneAction(packet, server);
                    break;
                
                case VIEW_APP:
                    viewAppelli(packet, server);
                    break;
                
                case VIEW_APP_P:
                    viewAppelliPrenotati(packet, server);
                    break;
                 
                 default:
                    packet.error.setAll(GENERIC, "Bad request", "Bad request sent inside packet. Value is "+packet.request);
                    server.Write((void*)&packet, sizeof(packet));
            }

            cout << endl;
            cout << "==============================" << endl;
            cout << endl;
        }
    }

    delete(&error);
    return 0;
}

// La funzione preleva la parte della data, la scompatta nei singoli componenti e poi li carica
// in un array. I componenti che vengono caricati sono posizionali, quindi se all'inizio della
// data si trova l'anno allora carica prima l'anno.
string formatDate(string date) {
    vector<string> date_parts, time_parts;
    string date_portion = date.substr(0, date.find(" "));
    string time_portion = date.substr(date.find(" ")+1);
    string result;
    
    for (int i = 0, position; i < 3; i++) {
        position = date_portion.find("-");
        if (position != -1) date_parts.push_back(date_portion.substr(0, position));
        else date_parts.push_back(date_portion);
        date_portion = date_portion.substr(position+1);
    }

    return date_parts.at(2) + "-" + date_parts.at(1) + "-" + date_parts.at(0) + " " + time_portion;
}

void loginAction(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;

    cout << "Matricola ricevuta: " << packet.data[MATRICOLA_STUDENTE] << endl;

    /* Errore che riguarda la connessione al DB*/
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    /* Ottenimento della matricola che sta tentando di autenticarsi */
    sql = "SELECT * FROM studenti WHERE matricola = '" + to_string(packet.data[MATRICOLA_STUDENTE]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {    // Errore che riguarda l'operazione di select al DB
        if (table->size() == 0) {
            error.setCode(AUTH_ERROR);
            cout << "La select non ha restituito nulla. La matricola non e' presente nel database" << endl;
        } else cout << "Matricola presente" << endl;
    } else error.printError();

    /* Invio errore (se è impostato a "OK" allora la matricola si è autenticata)*/
    packet.error = error;
    server.Write((void*)&packet, sizeof(packet));
    delete(table);
}

void insertAppelloAction(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    struct Appello appello_input;

    /* Errore che riguarda la connessione al DB*/
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        return;
    }

    // Legge i dati dell'appello
    server.Read((void*)&appello_input, sizeof(appello_input));
    cout << "Ecco i campi della struct Appello ricevuti" << endl;
    cout << "Codice appello: " << appello_input.codiceAppello << endl;
    cout << "Codice esame: " << appello_input.codiceEsame << endl;
    cout << "Data: " << appello_input.data << endl;
    cout << endl;

    /* Inserimento nel DB dell'appello */
    sql = "INSERT INTO appelli (CodiceAppello,CodiceEsame,Data) VALUES ('"
        + to_string(appello_input.codiceAppello) + "', '"
        + to_string(appello_input.codiceEsame) + "', '"
        + formatDate(appello_input.data) + "');";
    dbms.insert(sql, &error);

    /* Errore che riguarda la insert nel DB */
    if (error.getCode() != OK) error.printError();
    else cout << "Insert nel database andata a buon fine, appello inserito" << endl;

    /* Invio dati con errore (se l'errore non c'è allora sarà impostato a "OK") */
    packet.error = error;
    server.Write((void*)&packet, sizeof(packet));
}

void insertPrenotazioneAction(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    int number;

    cout << "Dati ricevuti" << endl;
    cout << "Codice appello: " << packet.data[APPELLO] << endl;
    cout << "Matricola: " << packet.data[MATRICOLA_STUDENTE] << endl;
    cout << endl;

    /* Errore che riguarda la connessione al DB*/
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    /* Ottenimento del numero di iscritti dal DB */
    sql = "SELECT count(*) AS MaxNumero FROM prenotazioni WHERE CodiceAppello = '" + to_string(packet.data[APPELLO]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {    // Errore che riguarda la select al DB
        /* Preparazione numero progressivo da associare alla matricola */
        number = stoi(table->at(0).at("MaxNumero")) + 1;
        cout << "Numero progressivo dell'ultimo prenotato: " << number - 1 << endl;
        cout << "Nuovo numero progressivo generato: " << number << endl;

        /* Inserimento della prenotazione */
        sql = "INSERT INTO prenotazioni VALUES ('"
            + to_string(packet.data[APPELLO]) + "', '"
            + to_string(packet.data[MATRICOLA_STUDENTE]) + "', '"
            + to_string(number) + "')";
        dbms.insert(sql, &error);

        /* Errore che riguarda la insert nel DB*/
        if (error.getCode() != OK) error.printError();
        else cout << "Inserimento andato a buon fine" << endl;
    } else error.printError();

    /* Invio errore (se è impostato a "OK" allora la prenotazione è stata inserita)*/
    packet.error = error;
    server.Write((void*)&packet, sizeof(packet));

    delete(table);
}

void viewAppelli(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct AppelloDisponibile *appelli; // Array che conterrà tutti i vari appelli

    cout << "Matricola ricevuta: " << packet.data[MATRICOLA_STUDENTE] << endl;
    cout << endl;

    /* Errore che riguarda la connessione al DB */
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    /* Ottenimento della tabella che contiene la lista degli appelli ai quali lo studente non è prenotato */
    sql = "SELECT appelli.CodiceAppello AS Codice, appelli.Data, corsi.Nome FROM appelli\
        JOIN esami ON appelli.CodiceEsame=esami.CodiceEsame\
        JOIN corsi ON esami.CodiceCorso=corsi.CodiceCorso\
        WHERE NOT EXISTS (SELECT prenotazioni.CodiceAppello\
            FROM prenotazioni\
            WHERE prenotazioni.Studente = '" + to_string(packet.data[MATRICOLA_STUDENTE]) + "'\
            AND prenotazioni.CodiceAppello=appelli.CodiceAppello)";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {    // Errore che riguarda la select al DB
        /* Preparazione tabella da inviare alla segreteria */
        appelli = (struct AppelloDisponibile*)malloc(sizeof(struct AppelloDisponibile) * table->size());
        cout << "Righe restituite: " << table->size() << endl;
        cout << endl;
        /* Salvataggio dati all'interno dell'array da inviare alla segreteria*/
        for (unsigned int i = 0; i < table->size(); i++) {
            map<string, string> row = table->at(i);
            appelli[i].codiceAppello = stoi(row.at("Codice"));
            strcpy(appelli[i].nome, row.at("Nome").c_str());
            strcpy(appelli[i].data, formatDate(row.at("Data")).c_str());

            cout << "Codice appello: " << appelli[i].codiceAppello << endl;
            cout << "Nome: " << appelli[i].nome << endl;
            cout << "Data: " << appelli[i].data << endl;
            cout << "----- ----- ----- -----" << endl;
        }

        /* Invio dati + errore (impostato a "OK" se è andato tutto a buon fine)*/
        packet.error = error;
        packet.data[RIGHE_QUERY] = table->size();
        server.Write((void*)&packet, sizeof(packet));   // Invio numero di righe (per preparare la dimensione del buffer)
        server.Write((void*)appelli, sizeof(struct AppelloDisponibile) * table->size());    // Invio effettivo della tabella
    } else {
        /* Errore che riuarda la select al DB */
        packet.error = error;
        error.printError();
        server.Write((void*)&packet, sizeof(packet));
    }

    delete(table);
    free(appelli);
}

void viewAppelliPrenotati(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct AppelloPrenotato *appelli;   // Array che conterrà la tabbella degli appelli ai quali lo studente è prenotato

    cout << "Matricola ricevuta: " << packet.data[MATRICOLA_STUDENTE] << endl;
    cout << endl;

    /* Errore che riguarda la connessione al DB */
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    /* Ottenimento degli appelli ai quali lo studente è prenotato */
    sql = "SELECT prenotazioni.CodiceAppello AS Codice,appelli.Data,corsi.Nome,esami.Tipo,esami.Modalita,esami.Descrizione, Prenotazioni.NumeroProgressivo AS NPrenotazione\
        FROM prenotazioni\
            JOIN studenti ON prenotazioni.Studente=studenti.Matricola\
            JOIN appelli ON prenotazioni.CodiceAppello=appelli.CodiceAppello\
            JOIN esami ON appelli.CodiceEsame=esami.CodiceEsame\
            JOIN corsi ON corsi.CodiceCorso=esami.CodiceCorso\
        WHERE prenotazioni.Studente = '" + to_string(packet.data[MATRICOLA_STUDENTE]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {    // Errore che riguarda la select al DB
        /* Preparazione tabella da inviare alla segreteria */
        appelli = (struct AppelloPrenotato*)malloc(sizeof(struct AppelloPrenotato) * table->size());
        cout << "Righe restituite: " << table->size() << endl;
        cout << endl;
        /* Salvataggio dati all'interno dell'array da inviare alla segreteria */
        for (unsigned int i = 0; i < table->size(); i++) {
            map<string, string> row = table->at(i);
            appelli[i].codice = stoi(row.at("Codice"));
            strcpy(appelli[i].data, formatDate(row.at("Data")).c_str());
            strcpy(appelli[i].nome, row.at("Nome").c_str());
            strcpy(appelli[i].tipo, row.at("Tipo").c_str());
            strcpy(appelli[i].modalita, row.at("Modalita").c_str());
            strcpy(appelli[i].descrizione, row.at("Descrizione").c_str());
            appelli[i].numeroPrenotazione = stoi(row.at("NPrenotazione"));

            cout << "Codice: " << appelli[i].codice << endl;
            cout << "Data: " << appelli[i].data << endl;
            cout << "Nome: " << appelli[i].nome << endl;
            cout << "Tipo: " << appelli[i].tipo << endl;
            cout << "Modalita: " << appelli[i].modalita << endl;
            cout << "Descrizione: " << appelli[i].descrizione << endl;
            cout << "Prenotazione: " << appelli[i].numeroPrenotazione << endl;
            cout << "----- ----- ----- -----" << endl;
        }

        /* Invio dati + errore (impostato a "OK" se è andato tutto a buon fine) */
        packet.error = error;
        packet.data[RIGHE_QUERY] = table->size();
        server.Write((void*)&packet, sizeof(packet));   // Invio numero di righe della tabella (per preparare il buffer)
        server.Write((void*)appelli, sizeof(struct AppelloPrenotato) * table->size());  // Invio effettivo della tabella
    } else {
        /* Errore che riguarda la select al DB */
        packet.error = error;
        error.printError();
        server.Write((void*)&packet, sizeof(packet));
    }

    delete(table);
    free(appelli);
}