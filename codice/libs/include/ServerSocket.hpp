#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "SocketCommunication.hpp"

//Classe per gestire il lato server della comunicazione:
//  -impostazione dei dati per l'ascolto
//  -messa in ascolto delle richeiste
//  -trasferimento dati attraverso le socket
class ServerSocket: protected SocketCommunication{
    private:
        int listen_fd;
        int connection_fd;
        sockaddr_in me;
        sockaddr_in client;
        socklen_t client_length;

    
        int Bind(int socket_fd,const struct sockaddr_in server_address);
        int Listen(int socket_fd, int queueLength);
    public:
        ServerSocket(int portToHost);
        void serverSetup();
        int Accept_NoClient();
        int Accept_NoClient(int socket_fd);
        int Accept_Client();
        int Accept_Client(int socket_fd,struct sockaddr_in *connected_client,socklen_t *client_lenght);
        void closeListening();
        void closeServer();
        ssize_t Read(void *buff,size_t n_bytes);
        ssize_t Write(const void *buff,size_t n_bytes);
};

#endif
