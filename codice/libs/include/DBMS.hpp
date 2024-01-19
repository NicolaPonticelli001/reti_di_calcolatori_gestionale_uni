#include "DataStructures.hpp"
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

using namespace std;

class DBMS {
private:
    /**
     * @brief Puntatore di tipo sqlite3 che contiene la connessione del database
     */
    sqlite3 *db;
    /**
     * @brief Valore intero che contiene l'eventuale errore di connessione al database
     */
    int error_connection_db;
    
    /**
     * @brief Crea un vettore di mappe che contiene i risultati della query sottoforma di map<string, string>.
     * 
     * @param table_result Puntatore del vettore all'interno del quale salvare salvare i risultati della query
     * sottoforma di map<string, string>
     */
    void createMapResults (vector< map<string, string> > *table_result, sqlite3_stmt*, Error* = nullptr);

    /**
     * @brief Wrapper per i metodi della libreria sqlite.h che permettono di eseguire l'effettiva query sul database
     * 
     * @param sql Query da eseguire
     * @param error_code Enumeratore ErrorType da restituire nel caso di errore
     * @param error Errore nel quale salvare l'esito della funzione
     * @param table_result Puntatore del vettore all'interno del quale salvare salvare i risultati della query
     * sottoforma di map<string, string>
     */
    void executeQuery (string sql, ErrorType error_code, Error *error, vector< map<string, string> > *table_result = nullptr);
public:
    /**
     * @brief Costruttore che instaura la connessione al database
     * 
     * @param db_name Nome del file con estensione .db (scrivere anche l'estensione)
     * @param error Errore nel quale salvare l'esito della connessione al database
     */
    DBMS (string db_name, Error *error = nullptr);

    /**
     * @brief Costruttore che distrugge la connessione al database
     * 
     */
    ~DBMS ();

    /**
     * @brief Funzione che effettua una select e restituisce la tabella sottoforma di vettore di mappe.
     * 
     * @param sql Query da eseguire
     * @param error Errore nel quale salvare l'sito della query
     * 
     * @return vector<map<string, string>* puntatore al vettore che contiene i singoli record sottoforma di map<string, string>
     */
    vector< map<string, string> >* select (string sql, Error *error = nullptr);

    /**
     * @brief Funzione che effettua una insert nel database
     * 
     * @param sql Query sql da eseguire
     * @param error Errore nel quale salvare l'esito della query
     */
    void insert (string sql, Error *error = nullptr);
};