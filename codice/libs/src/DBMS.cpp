#include "../include/DBMS.hpp"
#include <iostream>

using namespace std;

DBMS::DBMS(string db_name, Error *error = NULL) {
    int error_temp;
    error_temp = sqlite3_open(db_name.c_str(), &this->db);
    if(error_temp != SQLITE_OK) {
        if (error_temp == 0) error->setCode(OK);
        else {
            error->setCode(GENERIC);
            error->setTitle("SQLite connect " + error_temp);
            error->setDescription("Error while connecting to database \"" + db_name + "\"");
        }
    }

    this->error_connection_db = error_temp;
    this->table_results = vector< map<string, string> >();
}

DBMS::~DBMS() {
    sqlite3_close(this->db);
}

vector< map<string, string> > DBMS::select(string sql, Error *error = nullptr) {
    
    return this->table_results;
}

void DBMS::elaborateResults(void *additional_data, int count_data, char **data, char **column) {
    map<string, string> record;


    for(int i = 0; i < count_data; i++) {
        record[column[i]] = data[i];
    }

    table_results->push_back(record);
}

void DBMS::executeQuery(string sql, Error *error = NULL) {
    sqlite3_stmt *statement;
    int temp_error;
    char *temp_msg;

    if (this->error_connection_db != SQLITE_OK) {
        if (error != nullptr) {
            error->setAll(GENERIC, "SQLite connect " + this->error_connection_db, "Error while connecting to database");
        }
    } else {
        temp_error = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &statement, nullptr);
        if (temp_error != SQLITE_OK) {
            if (error != nullptr) {
                error->setAll(SELECT_ERROR, "SQL " + temp_error, "Error while preparing the statement - sqlite3_prepare_v2()");
            }
        } else {
            while ((temp_error = sqlite3_step(statement)) == SQLITE_ROW) {

            }
            if (temp_error != SQLITE_DONE) {
                if (error != nullptr) {
                    error->setAll(SELECT_ERROR, "SQL " + temp_error, "Error while obtaining rows - sqlite3_bind_text()");
                }
            }
        }
    }

    sqlite3_finalize(statement);
    return;

}