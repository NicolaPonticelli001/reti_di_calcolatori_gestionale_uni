#include <string>

using namespace std;

enum ErrorType {
    OK,             //0
    NOT_REC,        //1
    NOT_TRANSFER,   //2
    INSERT_ERROR,   //3
    SELECT_ERROR,   //4
    GENERIC         //5
};

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