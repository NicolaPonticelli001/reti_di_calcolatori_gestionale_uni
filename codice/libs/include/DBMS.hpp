#include "DataStructures.hpp"
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

using namespace std;

class DBMS {
private:
    sqlite3 *db;
    int error_connection_db;
    vector< map<string, string> > table_results;

    void elaborateResults(int, char**, char**);
    void executeQuery(string, Error*);
public:
    DBMS(string, Error* = NULL);
    ~DBMS();

    void connectDb(string, Error* = NULL);
    vector< map<string, string> > select(string, Error* = NULL);
};