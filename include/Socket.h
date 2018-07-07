#ifndef SOCKET_H
#define SOCKET_H

#include "InetAddress.h"

class Socket
{
    public:
        explicit Socket(int fd) : sockfd(fd) {};
        ~Socket();

        int fd() const { return sockfd; }
        void bindAddress(InetAddress& localAddr);
        void listen();
        int accept(InetAddress* peerAddr);
        void shutdownWrite();
        void setTcpNoDelay(bool on);
        void setReuseAddr(bool on);
        void setKeepAlive(bool on);

        static int createNonblockingSocket();

    private:
        int sockfd;
};

#endif // SOCKET_H
