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
void insertEsame(struct Packet packet, ServerSocket server);
void insertAppelloAction(struct Packet packet, ServerSocket server);
void insertPrenotazioneAction(struct Packet packet, ServerSocket server);
void viewCorsi(struct Packet packet, ServerSocket server);
void viewEsami(struct Packet packet, ServerSocket server);
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
                
                case INS_ESAME:
                    insertEsame(packet, server);
                    break;

                case INS_APPELLO:
                    insertAppelloAction(packet, server);
                    break;
                
                case PREN_STUD:
                    insertPrenotazioneAction(packet, server);
                    break;
                
                case VIEW_CORSI:
                    void viewCorsi(struct Packet packet, ServerSocket server);
                    break;
                
                case VIEW_ESAMI:
                    void viewEsami(struct Packet packet, ServerSocket server);
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

void insertEsame(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    struct Esame esame_input;

    /* Errore che riguarda la connessione al DB*/
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        return;
    }

    // Legge i dati dell'appello
    server.Read((void*)&esame_input, sizeof(esame_input));
    cout << "Ecco i campi della struct Appello ricevuti" << endl;
    cout << "Codice corso: " << esame_input.codiceCorso << endl;
    cout << "Tipo: " << esame_input.tipo << endl;
    cout << "Modalita: " << esame_input.modalita << endl;
    cout << "Descrizione: " << esame_input.descrizione << endl;
    cout << endl;

    /* Inserimento nel DB dell'appello */
    sql = "INSERT INTO Esami (CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('"
        + to_string(esame_input.codiceCorso) + "', '"
        + esame_input.tipo + "', '"
        + esame_input.modalita + "', '"
        + esame_input.descrizione + "')";
    dbms.insert(sql, &error);

    /* Errore che riguarda la insert nel DB */
    if (error.getCode() != OK) error.printError();
    else cout << "Insert nel database andata a buon fine, esame inserito" << endl;

    /* Invio dati con errore (se l'errore non c'è allora sarà impostato a "OK") */
    packet.error = error;
    server.Write((void*)&packet, sizeof(packet));
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
    cout << "Codice esame: " << appello_input.codiceEsame << endl;
    cout << "Data: " << appello_input.data << endl;
    cout << endl;

    /* Inserimento nel DB dell'appello */
    sql = "INSERT INTO appelli (CodiceEsame,Data) VALUES ('"
        + to_string(appello_input.codiceEsame) + "', '"
        + formatDate(appello_input.data) + "');";
    dbms.insert(sql, &error);

    /* Errore che riguarda la insert nel DB */
    if (error.getCode() != OK) error.printError();
    else cout << "Inserimento andato a buon fine" << endl;

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

void viewCorsi(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct Corso *corsi;

    /* Errore che riguarda la connessione al DB*/
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    /* Ottenimento dei corsi salvati nel DB */
    sql = "SELECT CodiceCorso AS Codice, Nome, CFU FROM corsi";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {    // Errore che riguarda la select al DB
        /* Preparazione tabella da inviare alla segreteria */
        corsi = (struct Corso*)malloc(sizeof(struct Corso) * table->size());
        cout << "Righe restituite: " << table->size() << endl;
        cout << endl;
        /* Salvataggio dati all'interno dell'array da inviare alla segreteria*/
        for (unsigned int i = 0; i < table->size(); i++) {
            map<string, string> row = table->at(i);
            corsi[i].codiceCorso = stoi(row.at("Codice"));
            strcpy(corsi[i].nome, row.at("Nome").c_str());
            corsi[i].CFU = stoi(row.at("CFU"));

            cout << "Codice corso: " << corsi[i].codiceCorso << endl;
            cout << "Nome: " << corsi[i].nome << endl;
            cout << "CFU: " << corsi[i].CFU << endl;
            cout << "----- ----- ----- -----" << endl;
        }

        /* Invio dati + errore (impostato a "OK" se è andato tutto a buon fine)*/
        packet.error = error;
        packet.data[RIGHE_QUERY] = table->size();
        server.Write((void*)&packet, sizeof(packet));   // Invio numero di righe (per preparare la dimensione del buffer)
        server.Write((void*)corsi, sizeof(struct Corso) * table->size());    // Invio effettivo della tabella
    } else {
        /* Errore che riuarda la select al DB */
        packet.error = error;
        error.printError();
        server.Write((void*)&packet, sizeof(packet));
    }

    delete(table);
    free(corsi);
}

void viewEsami(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct Esame *esami;

    cout << "Dati ricevuti" << endl;
    cout << "Corso: " << packet.data[CORSO] << endl;
    cout << endl;

    /* Errore che riguarda la connessione al DB*/
    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    /* Ottenimento dei corsi salvati nel DB */
    sql = "SELECT CodiceEsame AS Codice, Tipo, Modalita, Descrizione FROM esami WHERE CodiceCorso = '" + to_string(packet.data[CORSO]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {    // Errore che riguarda la select al DB
        /* Preparazione tabella da inviare alla segreteria */
        esami = (struct Esame*)malloc(sizeof(struct Esame) * table->size());
        cout << "Righe restituite: " << table->size() << endl;
        cout << endl;
        /* Salvataggio dati all'interno dell'array da inviare alla segreteria*/
        for (unsigned int i = 0; i < table->size(); i++) {
            map<string, string> row = table->at(i);
            esami[i].codiceEsame = stoi(row.at("Codice"));
            strcpy(esami[i].tipo, row.at("Tipo").c_str());
            strcpy(esami[i].modalita, row.at("Modalita").c_str());
            strcpy(esami[i].descrizione, row.at("Descrizione").c_str());

            cout << "Codice esame: " << esami[i].codiceEsame << endl;
            cout << "Tipo: " << esami[i].tipo << endl;
            cout << "Modalita: " << esami[i].modalita << endl;
            cout << "Descrizione: " << esami[i].descrizione << endl;
            cout << "----- ----- ----- -----" << endl;
        }

        /* Invio dati + errore (impostato a "OK" se è andato tutto a buon fine)*/
        packet.error = error;
        packet.data[RIGHE_QUERY] = table->size();
        server.Write((void*)&packet, sizeof(packet));   // Invio numero di righe (per preparare la dimensione del buffer)
        server.Write((void*)esami, sizeof(struct Esame) * table->size());    // Invio effettivo della tabella
    } else {
        /* Errore che riuarda la select al DB */
        packet.error = error;
        error.printError();
        server.Write((void*)&packet, sizeof(packet));
    }

    delete(table);
    free(esami);
}

void viewAppelli(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");    // Oggetto usato per tutti gli errori possibili
    DBMS dbms = DBMS(DB_FILE_NAME, &error); // Connessione al DB
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct AppelloDisponibile *appelli; // Array che conterrà tutti i vari appelli

    cout << "Dati ricevuti" << endl;
    cout << "Matricola: " << packet.data[MATRICOLA_STUDENTE] << endl;
    cout << "Codice corso: " << packet.data[ESAME] << endl;
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
            AND prenotazioni.CodiceAppello=appelli.CodiceAppello)\
        AND corsi.CodiceCorso = '" + to_string(packet.data[CORSO]) + "'";
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