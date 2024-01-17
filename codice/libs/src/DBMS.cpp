#include "../include/DBMS.hpp"
#include <iostream>

using namespace std;

DBMS::DBMS(string db_name, Error *error) {
    int error_temp;
    error_temp = sqlite3_open(db_name.c_str(), &this->db);
    if (error != nullptr) {
        if (error_temp == SQLITE_OK) error->setCode(OK);
        else error->setAll(GENERIC, "SQLite connect " + error_temp, "Error while connecting to database \"" + db_name + "\"");
    }

    this->error_connection_db = error_temp;
}

DBMS::~DBMS() {
    sqlite3_close(this->db);
}

vector< map<string, string> >* DBMS::select(string sql, Error *error) {
    vector< map<string, string> > *table_results = new vector< map<string, string> >();
    this->executeQuery(sql, error, table_results);
    return table_results;
}

void DBMS::executeQuery(string sql, Error *error = nullptr, vector< map<string, string> > *table_result) {
    sqlite3_stmt *statement;
    int temp_error;

    if (this->error_connection_db != SQLITE_OK) {
        if (error != nullptr) {
            error->setAll(GENERIC, "SQLite connect " + this->error_connection_db, "Error while connecting to database");
        }
    } else {
        temp_error = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &statement, nullptr);
        if (temp_error != SQLITE_OK) {
            if (error != nullptr) {
                error->setAll(SELECT_ERROR, "SQL " + temp_error, sqlite3_errmsg(this->db));
            }
        } else {

            if (table_result != nullptr) {
                this->createMapResults(table_result, statement, error);
            }
        }
    }

    sqlite3_finalize(statement);
    return;
}

void DBMS::createMapResults(vector< map<string, string> > *table_results, sqlite3_stmt *statement, Error *error) {
    map<string, string> row;
    int temp_error;

    while ((temp_error = sqlite3_step(statement)) == SQLITE_ROW) {
        const char *value;
        for (int i = 0; i < sqlite3_column_count(statement); i++) {
            value = reinterpret_cast<const char*>(sqlite3_column_text(statement, i));
            row[sqlite3_column_name(statement, i)] = (value == nullptr ? "" : value);
        }
        table_results->push_back(row);
    }
    if (temp_error != SQLITE_DONE) {
        if (error != nullptr) {
            error->setAll(SELECT_ERROR, "SQL " + temp_error, sqlite3_errmsg(this->db));
        }
    }
}