#include "../include/ClientSocket.hpp"

#include <iostream>

using namespace std;

//Costruttore, imposta l'IP e la porta a quelli del server a cui connettersi
ClientSocket::ClientSocket(string IP,int portToConnect) : SocketCommunication(portToConnect){
    this->server_IP=IP;
}

//Metodo che imposta i dati ed esegue le funzioni per permettere al client di connettersi con il server
void ClientSocket::clientSetup(){
    this->socket_fd=this->Socket(AF_INET,SOCK_STREAM,0);
    this->serverToConnect.sin_family=AF_INET;
    this->serverToConnect.sin_port=htons(this->port);
    if (inet_pton(AF_INET, this->server_IP.c_str(), &this->serverToConnect.sin_addr) < 0) {
        fprintf(stderr,"inet_pton error for %s\n", this->server_IP.c_str());
        exit (1);
    }
    cout<<"(ClientSocket: pronto per la connessione con "<<this->server_IP<<":"<<this->port<<")"<<endl;
}

//Metodi che wrappano la system call connect()
int ClientSocket::Connect(){
    cout<<"(ClientSocket: tentativo di connessione)"<<endl;
    if (connect(this->socket_fd, (struct sockaddr *) &this->serverToConnect, sizeof(serverToConnect)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    cout<<"(ClientSocket: connessione con "<<this->server_IP<<":"<<this->port<<" avvenuta con successo)"<<endl;
    return 0;
}

int ClientSocket::Connect(int socket_fd,const struct sockaddr_in server){
    cout<<"(ClientSocket: tentativo di connessione)"<<endl;
    if (connect(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    cout<<"(ClientSocket: connessione con "<<this->server_IP<<":"<<this->port<<" avvenuta con successo)"<<endl;
    return 0;
}

//Metodo che modifica l'indirizzo IP del server (salvandolo direttamente nella struct in formato network)
void ClientSocket::changeServerIP(string newIP){
    this->server_IP=newIP;
    if (inet_pton(AF_INET, this->server_IP.c_str(), &this->serverToConnect.sin_addr) < 0) {
        fprintf(stderr,"inet_pton error for %s\n", this->server_IP.c_str());
        exit (1);
    }
    cout<<"(ClientSocket: IP aggiornato con successo: "<<this->server_IP<<":"<<this->port<<")"<<endl;
}

//Metodo che chiude il file descriptor della socket
void ClientSocket::disconnect(){
    cout<<"(ClientSocket: chiusura connessione dal client)"<<endl;
    close(this->socket_fd);
}

//Metodi che wrappano le system calls read() e write()
ssize_t ClientSocket::Read(void *buf,size_t n_bytes){
    return FullRead(this->socket_fd,buf,n_bytes);
}

ssize_t ClientSocket::Write(const void *buf,size_t n_bytes){
    return FullWrite(this->socket_fd,buf,n_bytes);
}