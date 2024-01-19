#ifndef SOCKETCOMMUNICATION_HPP
#define SOCKETCOMMUNICATION_HPP

//Macro per indicare le porte di hosting e di connessione
#define UNI_SERVER_PORT 10000
#define SEGRETERIA_SERVER_PORT 20000

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>

class SocketCommunication{
    protected:
        int port;

        int Socket(int family,int type,int protocol);
    public:
        SocketCommunication(int port);
        ssize_t FullRead(int fd, void *buf, size_t count);
        ssize_t FullWrite(int fd, const void *buf, size_t count);
};

#endif
