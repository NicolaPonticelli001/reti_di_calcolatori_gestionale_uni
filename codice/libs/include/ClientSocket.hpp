#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "SocketCommunication.hpp"
#include <string>

//Classe per gestire il lato client della comunicazione:
//  -impostazione dei dati per la connessione
//  -richiesta di connessione al server
//  -trasferimento dati attraverso la socket
class ClientSocket: protected SocketCommunication{
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
        void changeServerPort(int portToConnect);
        void disconnect();
        ssize_t Read(void *buff,size_t n_bytes);
        ssize_t Write(const void *buff,size_t n_bytes);
};

#endif