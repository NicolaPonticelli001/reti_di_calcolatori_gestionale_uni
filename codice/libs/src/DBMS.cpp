#include "../include/DBMS.hpp"
#include <iostream>

using namespace std;

DBMS::DBMS(string db_name, Error *error) {
    int error_temp;

    error_temp = sqlite3_open(db_name.c_str(), &this->db);  // Connessione al database
    if (error != nullptr) {
        if (error_temp == SQLITE_OK) error->setCode(OK);
        else error->setAll(GENERIC, "SQLite connect " + error_temp, "Error while connecting to database \"" + db_name + "\"");
    }

    this->error_connection_db = error_temp;
}

// Distruttore
DBMS::~DBMS() {
    sqlite3_close(this->db);
}

/* Funzione che effettua una select e restituisce la tabella sottoforma di map<string, string>.
sql è la query da eseguire, error è l'oggetto nel quale salvare l'esito della query */
vector< map<string, string> >* DBMS::select(string sql, Error *error) {
    // Allocazione in memoria della mappa che verrà restituita
    vector< map<string, string> > *table_results = new vector< map<string, string> >();
    this->executeQuery(sql, SELECT_ERROR, error, table_results);
    return table_results;
}

void DBMS::executeQuery(string sql, ErrorType error_code, Error *error = nullptr, vector< map<string, string> > *table_result) {
    sqlite3_stmt *statement;
    int temp_error;

    // Verifica connessione al database
    if (this->error_connection_db != SQLITE_OK) {
        if (error != nullptr) {
            error->setAll(GENERIC, "SQLite connect " + this->error_connection_db, "Error while connecting to database");
        }
    } else {
        temp_error = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &statement, nullptr);    // Invio della query al database
        if (temp_error != SQLITE_OK) {
            if (error != nullptr) {
                error->setAll(error_code, "SQL " + to_string(temp_error), sqlite3_errmsg(this->db));
            }
        } else {

            if (table_result != nullptr) {
                this->createMapResults(table_result, statement, error); // Ottenimento dei risultati e salvataggio nel vettore di mappe
            } else {
                temp_error = sqlite3_step(statement);   // Esecuzione della query
                if (temp_error != SQLITE_OK && temp_error != 101) { // L'errore 101 si ha quando la query non restituisce i valori (è il caso delle insert)
                    if (error != nullptr) {
                        error->setAll(error_code, "SQL " + to_string(temp_error), sqlite3_errmsg(this->db));
                    }
                }
            }
        }
    }

    sqlite3_finalize(statement);
    return;
}

void DBMS::createMapResults(vector< map<string, string> > *table_results, sqlite3_stmt *statement, Error *error) {
    map<string, string> row;
    int temp_error;

    while ((temp_error = sqlite3_step(statement)) == SQLITE_ROW) {  // Ad ogni esecuzione di sqlite3_step() si ottiene una nuova riga
        const char *value;
        for (int i = 0; i < sqlite3_column_count(statement); i++) {
            value = reinterpret_cast<const char*>(sqlite3_column_text(statement, i));
            row[sqlite3_column_name(statement, i)] = (value == nullptr ? "" : value);
        }
        table_results->push_back(row);
    }

    /* Blocco da eseguire quando non ci sono più righe da iterare */
    if (temp_error != SQLITE_DONE) {
        if (error != nullptr) {
            error->setAll(SELECT_ERROR, "SQL " + temp_error, sqlite3_errmsg(this->db));
        }
    }
}

void DBMS::insert(string sql, Error *error) {
    this->executeQuery(sql, INSERT_ERROR, error, nullptr);
}