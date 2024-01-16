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
    char *temp_error;
    int temp_code;

    temp_code = sqlite3_exec(this->db, sql.c_str(), &DBMS::elaborateResults, &this->table_results, &temp_error);
    
    if(error != NULL) {
        if (temp_code == 0) error->setCode(OK);
        else {
            error->setCode(SELECT_ERROR);
            error->setTitle("SQL "+temp_code);
            error->setDescription(temp_error);
        }
    }

    return this->table_results;
}

int DBMS::elaborateResults(void *additional_data, int count_data, char **data, char **column) {
    vector< map<string, string> > *table_results = (vector< map<string, string> >*)additional_data;
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
            temp_error = sqlite3_bind_text(statement, 1, parametro.c_str(), -1, SQLITE_STATIC);
            if (temp_error != SQLITE_OK) {
                if (error != nullptr) {
                    error->setAll(SELECT_ERROR, "SQL " + temp_error, "Error while binding - sqlite3_bind_text()");  
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
    }

    sqlite3_finalize(statement);
    return;

}