#ifndef SOCKETCOMMUNICATION_HPP
#define SOCKETCOMMUNICATION_HPP

//Da mettere in "DataStructures.hpp"
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
        int Socket(int , int , int );

    public:
        SocketCommunication(int );
        ssize_t FullRead(int fd, void *buf, size_t count);
        ssize_t FullWrite(int fd, const void *buf, size_t count);
};

#endif
