#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

#include <string>
#include <vector>

using namespace std;

//Defines per accedere direttamente al campo "data" della struct "Packet"
#define MATRICOLA_STUDENTE 0
#define APPELLO 1
#define RIGHE_QUERY 2
#define GENERIC_DATA 3   //Per il numero progressivo

//Enumerazione per identificare il tipo di richiesta
enum RequestType{
    LOGIN,          //0
    INS_APPELLO,    //1
    PREN_STUD,      //2
    VIEW_APP,       //3
    VIEW_APP_P      //4
};

//Enumerazione per identificare il tipo di errore
enum ErrorType {
    OK,             //0
    NOT_REC,        //1
    NOT_TRANSFER,   //2
    INSERT_ERROR,   //3
    SELECT_ERROR,   //4
    AUTH_ERROR,     //5
    GENERIC         //6
};

//Classe che rappresenta l'errore che si è verificato dopo una richiesta
class Error {
    private:
        ErrorType code;
        char title[50], description[500];

    public:
        Error();
        Error(ErrorType error_code);
        Error(ErrorType error_code, string title, string description);
        ~Error();

        void setAll(ErrorType error_code, string title, string description);
        void setCode(ErrorType error_code);
        ErrorType getCode();
        void setTitle(string title);
        string getTitle();
        void setDescription(string description);
        string getDescription();
        void printError();
};

//Struttura utilizzata come pacchetto per inviare la richiesta e ricevere risposte dal server ed eventuali errori
struct Packet{
    RequestType request = LOGIN;
    Error error;
    int data[4];
};

//Stutture utilizzate per le tabelle
struct Appello{                 //struct di input
    int codiceAppello;
    int codiceEsame;
    char data[17];
};

struct AppelloPrenotato{        //struct di output
    int codice;
    char data[16];
    char nome[50];
    char tipo[5];
    char modalita[7];
    char descrizione[500];
    short int numeroPrenotazione;
};

struct AppelloDisponibile{      //struct di output
    int codiceAppello;
    char data[16];
    char nome[50];
};

#endif