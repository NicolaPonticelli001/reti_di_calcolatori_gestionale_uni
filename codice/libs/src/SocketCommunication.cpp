#include "../include/SocketCommunication.hpp"

SocketCommunication::SocketCommunication(int porta){
    this->port=porta;
}

int SocketCommunication::Socket(int family, int type, int protocol){
    int fd;
    if((fd=socket(family,type,protocol))<0){
        perror("Socket");
        exit(1);
    }
    return fd;
}

ssize_t SocketCommunication::FullRead(int fd, void *buf, size_t count){
    size_t nleft; 
    ssize_t nread; 
    nleft = count; 
    while (nleft > 0){ /* repeat until no left */
        if ( (nread = read(fd, buf, nleft)) < 0){
            if (errno == EINTR){   /* if interrupted by system call */
                continue; /* repeat the loop */  
            }
            else {
                exit(nread);      /* otherwise exit */  
            }
        }
        else{
            if(nread == 0){    /* EOF */  
                break;         /* break loop here */  
            }
        }
        nleft -= nread;      /* set left to read */  
        buf += nread;        /* set pointer */  
    }
    buf = 0;
    return (nleft);
}

ssize_t SocketCommunication::FullWrite(int fd, const void *buf, size_t count){
    size_t nleft;
    ssize_t nwritten;
    nleft = count;
    while(nleft > 0){  /* repeat until no left */
        if ( (nwritten = write(fd, buf, nleft)) < 0){
            if (errno == EINTR){  /* if interrupted by system call */
                continue;   /* repeat the loop */
            }
            else{
                exit(nwritten);  /* otherwise exit with error */
            }
        }
        nleft -= nwritten;  /* set left to write */
        buf += nwritten;  /* set pointer */
    }
    return (nleft);

}