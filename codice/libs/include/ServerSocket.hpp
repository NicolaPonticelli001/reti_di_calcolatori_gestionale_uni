#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "SocketCommunication.hpp"

class ServerSocket: public SocketCommunication{
    private:
        int listen_fd;
        int connection_fd;
        sockaddr_in me;
        sockaddr_in client;
        socklen_t client_length;

    
        int Bind(int , const struct sockaddr_in );
        int Listen(int , int );
    public:
        ServerSocket(int );
        void serverSetup();
        int Accept_NoClient();
        int Accept_NoClient(int );
        int Accept_Client();
        int Accept_Client(int ,struct sockaddr_in *,socklen_t *);
        ssize_t Read(void *buff);
        ssize_t Write(const void *buff);
};

#endif