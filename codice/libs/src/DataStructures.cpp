#include "../include/DataStructures.hpp"
#include <iostream>
#include <cstring>

using namespace std;

Error::Error() {}
Error::Error(ErrorType error_code) {
    this->setCode(error_code);
}
Error::Error(ErrorType error_code, string title, string description) {
    this->setAll(error_code, title, description);
}
Error::~Error() {}

void Error::setAll(ErrorType error_code, string title, string description) {
    this->setCode(error_code);
    this->setTitle(title);
    this->setDescription(description);
}

void Error::setCode(ErrorType error_code) {this->code = error_code;}
ErrorType Error::getCode() {return this->code;}

void Error::setTitle(string title) {strncpy(this->title, title.c_str(), title.size());}
string Error::getTitle() {return this->title;}

void Error::setDescription(string description) {strncpy(this->description, description.c_str(), description.size());}
string Error::getDescription() {return this->description;}

void Error::printError() {
    cout << "ERRORE" << endl;
    cout << "Codice: " << this->code << endl;
    cout << this->title << ": " << this->description << endl;
}