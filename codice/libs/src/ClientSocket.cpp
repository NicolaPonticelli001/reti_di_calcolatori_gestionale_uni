#include "../include/ClientSocket.hpp"

#include <iostream>

using namespace std;

ClientSocket::ClientSocket(string IP,int portToConnect) : SocketCommunication(portToConnect){
    this->server_IP=IP;
}

void ClientSocket::clientSetup(){
    this->socket_fd=this->Socket(AF_INET,SOCK_STREAM,0);
    this->server.sin_family=AF_INET;
    this->server.sin_port=htons(this->port);
    if (inet_pton(AF_INET, this->server_IP.c_str(), &this->server.sin_addr) < 0) {
        fprintf(stderr,"inet_pton error for %s\n", this->server_IP.c_str());
        exit (1);
    }
    cout<<"Il client e' pronto per la connessione"<<endl;
}

int ClientSocket::Connect(){
    cout<<"Avvio tentativo di connessione"<<endl;
    if (connect(this->socket_fd, (struct sockaddr *) &this->server, sizeof(server)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    cout<<"Connessione avvenuta con successo"<<endl;
    return 0;
}

int ClientSocket::Connect(int socket_fd,const struct sockaddr_in server_address){
    cout<<"Avvio tentativo di connessione"<<endl;
    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    cout<<"Connessione avvenuta con successo"<<endl;
    return 0;
}

void ClientSocket::changeServerIP(string newIP){
    this->server_IP=newIP;
    if (inet_pton(AF_INET, this->server_IP.c_str(), &this->server.sin_addr) < 0) {
        fprintf(stderr,"inet_pton error for %s\n", this->server_IP.c_str());
        exit (1);
    }
    cout<<"IP aggiornato con successo"<<endl;
}

void ClientSocket::disconnect(){
    close(this->socket_fd);
}

ssize_t ClientSocket::Read(void *buf,size_t n_bytes){
    return FullRead(this->socket_fd,buf,n_bytes);
}

ssize_t ClientSocket::Write(const void *buf,size_t n_bytes){
    return FullWrite(this->socket_fd,buf,n_bytes);
}