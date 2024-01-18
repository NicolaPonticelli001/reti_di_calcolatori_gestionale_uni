#include "../include/DataStructures.hpp"
#include <iostream>
#include <cstring>

using namespace std;

Error::Error() {}

Error::~Error() {}

Error::Error(ErrorType error_code) {
    this->setCode(error_code);
}

Error::Error(ErrorType error_code, string title, string description) {
    this->setAll(error_code, title, description);
}

void Error::setAll(ErrorType error_code, string title, string description) {
    this->setCode(error_code);
    this->setTitle(title);
    this->setDescription(description);
}

void Error::setCode(ErrorType error_code) {
    this->code = error_code;
}

ErrorType Error::getCode() {
    return this->code;
}

void Error::setTitle(string title) {
    strncpy(this->title, title.c_str(), title.size());
}

string Error::getTitle() {
    return this->title;
}

void Error::setDescription(string description) {
    strncpy(this->description, description.c_str(), description.size());
}

string Error::getDescription() {
    return this->description;
}

void Error::printError() {
    string title_str = this->title, description_str = this->description;

    cout << endl << "Errore codice " << this->code << endl;
    if (!title_str.empty()) cout << "Titolo: " << title_str << endl;
    if (!description_str.empty()) cout << "Descrizione: " << description_str << endl;
    cout << endl;
}