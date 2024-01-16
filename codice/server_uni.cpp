#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

static int printTable(void *additional_data, int count, char **data, char **column) {
    int *records = (int*) additional_data;

    for(int i = 0; i<count; i++) {
        cout<<column[i]<<": "<< ((data[i] == NULL) ? "" : data[i])<< endl;
    }
    cout<<endl;

    (*records)++;
    return 0;
}

int main() {
    sqlite3 *database;
    int sql_result, records_number = 0;
    string db_name = "universita.db", sql;
    char *error_message;
    vector<vector<string>> sql_inserts = {
        {"INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('1', 'Aldo', 'Aldi');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('2', 'Inès', 'De Lacroix');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('3', 'Niky', 'Ponty');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('4', 'Frank', 'Pelly');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('5', 'Sonia', 'Sincoria');"},
        {"INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('1', 'Architettura dei Calcolatori', '12');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('2', 'Programmazione III', '6');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('3', 'Reti di Calcolatori', '9');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('4', 'Tecnologie Web', '6');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('5', 'Algoritmi e Strutture Dati', '12');"},
        {"INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('1', '1', 'Esame', 'Orale', NULL);","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('2', '2', 'Esame', 'Orale', 'Esame orale composto da due parti:\r\n-Discussione del progetto\r\n-Prova orale sugli argomenti del corso');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('3', '3', 'Esame', 'Orale', 'Esame orale composto da due parti:\r\n-Discussione del progetto\r\n-Prova orale sugli argomenti del corso');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('4', '5', 'Prova', 'Scritto', 'Prova scritta');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('5', '5', 'Prova', 'Scritto', 'Prova pratica C++');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('6', '5', 'Esame', 'Orale', NULL);","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('7', '1', 'Esame', 'Scritto', 'Per accedere alla prova orale bisogna superare la prova scritta');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('8', '4', 'Esame', 'Orale', NULL);"},
        {"INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('1', '1', '2024-01-15 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('2', '7', '2024-01-11 09:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('3', '7', '2024-01-30 09:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('4', '1', '2024-02-05 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('5', '4', '2024-01-10 14:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('6', '5', '2024-01-17 14:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('7', '6', '2024-01-25 09:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('8', '2', '2024-01-22 09:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('9', '2', '2024-02-19 09:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('10', '3', '2024-01-25 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('11', '3', '2024-02-15 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('12', '8', '2024-01-08 10:00:00');"},
        {"INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('1', '1', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('1', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('3', '5', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('4', '5', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('9', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('9', '4', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '4', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '3', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '2', '3');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '5', '4');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('8', '1', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('11', '1', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('5', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('6', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('7', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('5', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('6', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('7', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('7', '1', '3');"}
    };
    
    sql_result = sqlite3_open(db_name.c_str(), &database);
    
    // for(unsigned int i = 0; i<sql_inserts.size(); i++) {
    //     for(unsigned int j = 0; j<sql_inserts.at(i).size(); j++) {
    //         sql_result = sqlite3_exec(database, sql_inserts.at(i).at(j).c_str(), printTable, 0, &error_message);
    //         if (sql_result) {
    //             cout << "Errore query: " << sql_inserts.at(i).at(j) << endl;
    //             cout << sql_result << ": \"" << error_message << "\"" << endl << endl;
    //         }
    //     }
    // }
    
    sql = "SELECT * FROM esami";
    records_number = 0;
    sql_result = sqlite3_exec(database, sql.c_str(), printTable, &records_number, &error_message);
    if (sql_result) cout <<"Errore " << sql_result << ": \"" << error_message << "\"" << endl << endl;
    cout << "Records: " << records_number << endl;

    sqlite3_close(database);
    return 0;
}