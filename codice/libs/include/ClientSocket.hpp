#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "SocketCommunication.hpp"
#include <string>

class ClientSocket: public SocketCommunication{
    private:
        std::string server_IP;
        int socket_fd;
        sockaddr_in serverToConnect;

    public:
        ClientSocket(std::string serverIP,int portToConnect);
        void clientSetup();
        int Connect();
        int Connect(int fd,const struct sockaddr_in server);
        void changeServerIP(std::string newIP);
        void disconnect();
        ssize_t Read(void *buff,size_t n_bytes);
        ssize_t Write(const void *buff,size_t n_bytes);
};

#endif