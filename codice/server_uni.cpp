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
#define DB_FILE_NAME "universita.db"

using namespace std;

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
        pid = fork();

        if (pid < 0) {
            packet.error.setAll(GENERIC, "Fork", "Error while creating child process (pid < 0)");
            server.Write((void*)&packet, sizeof(packet));
        } else if (pid == 0) {
            server.Read((void*)&packet, sizeof(packet));
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
        }
    }

    delete(&error);
    return 0;
}

string formatDate(string date) {
    vector<string> date_parts, time_parts;
    string date_portion = date.substr(0, date.find(" "));
    string time_portion = date.substr(date.find(" ")+1);
    
    for (int i = 0, position; i < 3; i++) {
        position = date_portion.find("-");
        if (position != -1) date_parts.push_back(date_portion.substr(0, position));
        else date_parts.push_back(date_portion);
        date_portion = date_portion.substr(position+1);
    }

    return date_parts.at(2) + "-" + date_parts.at(1) + "-" + date_parts.at(0) + " " + time_portion;
}

void loginAction(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");
    DBMS dbms = DBMS(DB_FILE_NAME, &error);
    string sql = "";
    vector< map<string, string> > *table = nullptr;

    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    sql = "SELECT * FROM studenti WHERE matricola = '" + to_string(packet.data[MATRICOLA_STUDENTE]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {
        if (table->size() == 0) error.setCode(AUTH_ERROR);
    } else error.printError();

    packet.error = error;
    server.Write((void*)&packet, sizeof(packet));
    delete(table);
}

void insertAppelloAction(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");
    DBMS dbms = DBMS(DB_FILE_NAME, &error);
    string sql = "";
    struct Appello appello_input;

    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        return;
    }

    server.Read((void*)&appello_input, sizeof(appello_input));

    sql = "INSERT INTO appelli (CodiceAppello,CodiceEsame,Data) VALUES ('"
        + to_string(appello_input.codiceAppello) + "', '"
        + to_string(appello_input.codiceEsame) + "', '"
        + formatDate(appello_input.data) + "');";
    dbms.insert(sql, &error);
    if (error.getCode() != OK) error.printError();

    packet.error = error;
    server.Write((void*)&packet, sizeof(packet)); 
}

void insertPrenotazioneAction(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");
    DBMS dbms = DBMS(DB_FILE_NAME, &error);
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    int number = -1;

    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    sql = "SELECT MAX(NumeroProgressivo) AS MaxNumero FROM prenotazioni WHERE CodiceAppello = '" + to_string(packet.data[APPELLO]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {
        number = stoi(table->at(0).at("MaxNumero")) + 1;

        sql = "INSERT INTO prenotazioni VALUES ('"
            + to_string(packet.data[APPELLO]) + "', '"
            + to_string(packet.data[MATRICOLA_STUDENTE]) + "', '"
            + to_string(number) + "')";
        dbms.insert(sql, &error);
    } else error.printError();

    packet.error = error;
    if (number != -1) packet.data[GENERIC_DATA] = number;   // Se c'è il numero progressivo
    server.Write((void*)&packet, sizeof(packet));

    delete(table);
}

void viewAppelli(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");
    DBMS dbms = DBMS(DB_FILE_NAME, &error);
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct AppelloDisponibile *appelli;

    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    sql = "SELECT appelli.CodiceAppello AS Codice, appelli.Data, corsi.Nome FROM appelli\
        JOIN esami ON appelli.CodiceEsame=esami.CodiceEsame\
        JOIN corsi ON esami.CodiceCorso=corsi.CodiceCorso\
        WHERE NOT EXISTS (SELECT prenotazioni.CodiceAppello\
            FROM prenotazioni\
            WHERE prenotazioni.Studente = '" + to_string(packet.data[MATRICOLA_STUDENTE]) + "'\
            AND prenotazioni.CodiceAppello=appelli.CodiceAppello)";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {
        appelli = (struct AppelloDisponibile*)malloc(sizeof(struct AppelloDisponibile) * table->size());
        for (unsigned int i = 0; i < table->size(); i++) {
            map<string, string> row = table->at(i);
            appelli[i].codiceAppello = stoi(row.at("Codice"));
            strcpy(appelli[i].data, formatDate(row.at("Data")).c_str());
            strcpy(appelli[i].nome, row.at("Nome").c_str());
        }
        packet.error = error;
        packet.data[RIGHE_QUERY] = table->size();
        server.Write((void*)&packet, sizeof(packet));
        server.Write((void*)&appelli, sizeof(struct AppelloDisponibile) * table->size());
    } else {
        packet.error = error;
        error.printError();
        server.Write((void*)&packet, sizeof(packet));
    }

    delete(table);
    free(appelli);
}

void viewAppelliPrenotati(struct Packet packet, ServerSocket server) {
    Error error = Error(OK, "", "");
    DBMS dbms = DBMS(DB_FILE_NAME, &error);
    string sql = "";
    vector< map<string, string> > *table = nullptr;
    struct AppelloPrenotato *appelli;

    if (error.getCode() != OK) {
        error.printError();
        packet.error = error;
        server.Write((void*)&packet, sizeof(packet));
        delete(table);
        return;
    }

    sql = "SELECT prenotazioni.CodiceAppello AS Codice,appelli.Data,corsi.Nome,esami.Tipo,esami.Modalita,esami.Descrizione, Prenotazioni.NumeroProgressivo AS NumeroPrenotazione\
        FROM prenotazioni\
            JOIN studenti ON prenotazioni.Studente=studenti.Matricola\
            JOIN appelli ON prenotazioni.CodiceAppello=appelli.CodiceAppello\
            JOIN esami ON appelli.CodiceEsame=esami.CodiceEsame\
            JOIN corsi ON corsi.CodiceCorso=esami.CodiceCorso\
        WHERE prenotazioni.Studente = '" + to_string(packet.data[MATRICOLA_STUDENTE]) + "'";
    table = dbms.select(sql, &error);

    if (error.getCode() == OK) {
        appelli = (struct AppelloPrenotato*)malloc(sizeof(struct AppelloPrenotato) * table->size());
        for (unsigned int i = 0; i < table->size(); i++) {
            map<string, string> row = table->at(i);
            appelli[i].codice = stoi(row.at("Codice"));
            strcpy(appelli[i].data, formatDate(row.at("Data")).c_str());
            strcpy(appelli[i].nome, row.at("Nome").c_str());
            strcpy(appelli[i].tipo, row.at("Tipo").c_str());
            strcpy(appelli[i].modalita, row.at("Modalita").c_str());
            strcpy(appelli[i].descrizione, row.at("Descrizione").c_str());
            appelli[i].numeroPrenotazione = stoi(row.at("NumeroPrenotazione"));
        }
        packet.error = error;
        packet.data[RIGHE_QUERY] = table->size();
        server.Write((void*)&packet, sizeof(packet));
        server.Write((void*)&appelli, sizeof(struct AppelloPrenotato) * table->size());
    } else {
        packet.error = error;
        error.printError();
        server.Write((void*)&packet, sizeof(packet));
    }

    delete(table);
    free(appelli);
}