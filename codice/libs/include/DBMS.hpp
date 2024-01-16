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

    void createMapResults(vector< map<string, string> >*, sqlite3_stmt*, Error* = nullptr);
    void executeQuery(string, Error*, vector< map<string, string> >* = nullptr);
public:
    DBMS(string, Error* = nullptr);
    ~DBMS();

    vector< map<string, string> >* select(string, Error* = nullptr);
};