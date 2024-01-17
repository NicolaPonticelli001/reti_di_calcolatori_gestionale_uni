#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "SocketCommunication.hpp"
#include <string>

class ClientSocket: public SocketCommunication{
    private:
        std::string server_IP;
        int socket_fd;
        sockaddr_in server;

    public:
        ClientSocket(std::string ,int );
        void clientSetup();
        int Connect();
        int Connect(int ,const struct sockaddr_in );
        void changeServerIP(std::string );
        ssize_t Read(void *buff);
        ssize_t Write(const void *buff);
};

#endif