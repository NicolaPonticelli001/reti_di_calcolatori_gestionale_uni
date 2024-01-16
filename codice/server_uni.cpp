#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>
#include "libs/include/DBMS.hpp"

using namespace std;

int main() {
    string db_name = "universita.db";
    string sql;
    vector<vector<string>> sql_inserts = {
        {"INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('1', 'Aldo', 'Aldi');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('2', 'Inès', 'De Lacroix');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('3', 'Niky', 'Ponty');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('4', 'Frank', 'Pelly');","INSERT INTO studenti (Matricola, Nome, Cognome) VALUES ('5', 'Sonia', 'Sincoria');"},
        {"INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('1', 'Architettura dei Calcolatori', '12');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('2', 'Programmazione III', '6');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('3', 'Reti di Calcolatori', '9');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('4', 'Tecnologie Web', '6');","INSERT INTO corsi (CodiceCorso, Nome, CFU) VALUES ('5', 'Algoritmi e Strutture Dati', '12');"},
        {"INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('1', '1', 'Esame', 'Orale', NULL);","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('2', '2', 'Esame', 'Orale', 'Esame orale composto da due parti:\r\n-Discussione del progetto\r\n-Prova orale sugli argomenti del corso');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('3', '3', 'Esame', 'Orale', 'Esame orale composto da due parti:\r\n-Discussione del progetto\r\n-Prova orale sugli argomenti del corso');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('4', '5', 'Prova', 'Scritto', 'Prova scritta');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('5', '5', 'Prova', 'Scritto', 'Prova pratica C++');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('6', '5', 'Esame', 'Orale', NULL);","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('7', '1', 'Esame', 'Scritto', 'Per accedere alla prova orale bisogna superare la prova scritta');","INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('8', '4', 'Esame', 'Orale', NULL);"},
        {"INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('1', '1', '2024-01-15 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('2', '7', '2024-01-11 09:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('3', '7', '2024-01-30 09:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('4', '1', '2024-02-05 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('5', '4', '2024-01-10 14:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('6', '5', '2024-01-17 14:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('7', '6', '2024-01-25 09:30:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('8', '2', '2024-01-22 09:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('9', '2', '2024-02-19 09:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('10', '3', '2024-01-25 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('11', '3', '2024-02-15 10:00:00');","INSERT INTO appelli (CodiceAppello, CodiceEsame, Data) VALUES ('12', '8', '2024-01-08 10:00:00');"},
        {"INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('1', '1', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('1', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('3', '5', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('4', '5', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('9', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('9', '4', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '4', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '3', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '2', '3');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('10', '5', '4');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('8', '1', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('11', '1', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('5', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('6', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('7', '3', '1');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('5', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('6', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('7', '2', '2');","INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('7', '1', '3');"}
    };
    Error *error = new Error();
    DBMS *dbms;
    vector< map<string, string> > *table;

    dbms = new DBMS(db_name, error);
    error->printError();
    cout<<endl;

    sql = "select * from appelli";
    table = dbms->select(sql, error);
    error->printError();
    cout << endl << endl;

    cout << "Elementi: " << table->size() << endl;
    for(unsigned int i = 0; i < table->size(); i++) {
        map<string, string> row = table->at(i);
        for(auto j = row.begin(); j != row.end(); j++) {
            cout << j->first << ": " << j->second << endl;
        }

        cout<<endl;
    }
    error->printError();
    cout << endl << endl;

    delete(error);
    delete(dbms);
    return 0;
}