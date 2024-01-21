#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

#include <string>
#include <vector>

using namespace std;

//Defines per accedere direttamente al campo "data" della struct "Packet"
#define MATRICOLA_STUDENTE 0
#define CORSO 1
#define ESAME 2
#define APPELLO 3
#define RIGHE_QUERY 4
#define GENERIC_DATA 5   //Per il numero progressivo

//Enumerazione per identificare il tipo di richiesta
enum RequestType{
    LOGIN,          //0
    INS_APPELLO,    //1
    INS_ESAME,      //2
    PREN_STUD,      //3
    VIEW_CORSI,     //4
    VIEW_ESAMI,     //5
    VIEW_APP,       //6 
    VIEW_APP_P      //7
};

//Enumerazione per identificare il tipo di errore
enum ErrorType {
    OK,             //0
    INSERT_ERROR,   //1
    SELECT_ERROR,   //2
    AUTH_ERROR,     //3
    GENERIC         //4
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
    int data[5];
};

//Stutture utilizzate per le tabelle
struct Appello{                 //struct di input
    int codiceAppello;
    int codiceEsame;
    char data[17];
};

struct Esame{                   //struct di input/output
    int codiceEsame;
    int codiceCorso;
    char tipo[6];
    char modalita[8];
    char descrizione[501];
};

struct AppelloPrenotato{        //struct di output
    int codice;
    char data[17];
    char nome[51];
    char tipo[6];
    char modalita[8];
    char descrizione[501];
    short int numeroPrenotazione;
};

struct AppelloDisponibile{      //struct di output
    int codiceAppello;
    char data[17];
    char nome[51];
};

struct Corso{                   //struct di output
    int codiceCorso;
    char nome[51];
    short int CFU;
};

#endif