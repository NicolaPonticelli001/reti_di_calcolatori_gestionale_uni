#include "../include/ServerSocket.hpp"

#include <iostream>
#include <string>

using namespace std;

ServerSocket::ServerSocket(int portToHost) : SocketCommunication(portToHost){
}

void ServerSocket::serverSetup(){
    listen_fd=Socket(AF_INET,SOCK_STREAM,0);
    int flag = 1;
    setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    me.sin_family=AF_INET;
    me.sin_addr.s_addr=htonl(INADDR_ANY);
    me.sin_port=htons(this->port);
    Bind(listen_fd,me);
    Listen(listen_fd,1024);
    cout<<"Server pronto!"<<endl;
    cout<<"In attesa..."<<endl;
}

int ServerSocket::Bind(int socket_fd, const struct sockaddr_in server_address){
    if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 ) {
        perror("bind");
        exit(1);
    }
    return 0;
}

int ServerSocket::Listen(int socket_fd, int lunghezza_coda){
    if ( listen(socket_fd, lunghezza_coda) < 0 ) {
        perror("listen");
        exit(1);
    }
    return 0;
}

int ServerSocket::Accept_NoClient(){
    if ( ( this->connection_fd = accept(this->listen_fd, (struct sockaddr *) NULL, NULL) ) < 0 ) {
        perror("accept");
        exit(1);
    }
    cout<<"Client accettato"<<endl;
    return this->connection_fd;
}

int ServerSocket::Accept_NoClient(int socket_fd){
    int connection_fd;
    if ( ( connection_fd = accept(socket_fd, (struct sockaddr *) NULL, NULL) ) < 0 ) {
        perror("accept");
        exit(1);
    }
    return connection_fd;
}

int ServerSocket::Accept_Client(){
    this->client_length=sizeof(this->client);
    int connection_fd;
    if ( ( connection_fd = accept(this->listen_fd, (struct sockaddr *) &this->client, &this->client_length) ) < 0 ) {
        perror("accept");
        exit(1);
    }
    return connection_fd;
}

int ServerSocket::Accept_Client(int socket_fd,struct sockaddr_in *connected_client,socklen_t *client_lenght){
    int connection_fd;
    if ( ( connection_fd = accept(socket_fd, (struct sockaddr *) connected_client, client_lenght) ) < 0 ) {
        perror("accept");
        exit(1);
    }
    return connection_fd;
}

void ServerSocket::closeListening(){
    close(this->listen_fd);
}

void ServerSocket::closeServer(){
    close(this->listen_fd);
}

ssize_t ServerSocket::Read(void *buf,size_t n_bytes){
    return FullRead(this->connection_fd,buf,n_bytes);
}

ssize_t ServerSocket::Write(const void *buf,size_t n_bytes){
    return FullWrite(this->connection_fd,buf,n_bytes);
}