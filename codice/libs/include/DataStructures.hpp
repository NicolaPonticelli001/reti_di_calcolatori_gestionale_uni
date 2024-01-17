#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

#include <string>

using namespace std;

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

//Struttura utilizzata come pacchetto per inviare la richiesta e ricevere risposte dal server ed eventuali errori
struct packet{
    RequestType request;
    Error error;
    int data[4];
};

//Classe che rappresenta l'errore che si è verificato dopo una richiesta
class Error {
    private:
        ErrorType code;
        char title[50], description[500];

    public:
        Error();
        Error(ErrorType);
        Error(ErrorType, string, string);
        ~Error();

        void setAll(ErrorType, string, string);
        void setCode(ErrorType);
        ErrorType getCode();
        void setTitle(string);
        string getTitle();
        void setDescription(string);
        string getDescription();
        void printError();
};

//Stutture utilizzate per le tabelle
struct Appello{                 //struct di input
    int codiceAppello;
    int codiceEsame;
    char data[16];
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