#include "Socket.h"
#include <unistd.h>
#include <iostream>
#include <netinet/tcp.h>
#include <string.h>
#include<fcntl.h>

void setNonBlockAndCloseOnExec(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flag);

    flag = fcntl(fd, F_GETFD, 0);
    flag |= FD_CLOEXEC;
    ret = fcntl(fd, F_SETFD, flag);

    (void)ret;//消除编译时警告
}

Socket::~Socket()
{
    close(sockfd);
}

void Socket::bindAddress(InetAddress& localAddr)
{
    struct sockaddr_in addrin = localAddr.getSockAddr();
    int ret = bind(sockfd, (struct sockaddr*)&addrin, sizeof addrin);
    if(ret < 0)
    {
        //error
        std::cerr << "bind error" << std::endl;
        exit(0);
    }
}

void Socket::listen()
{
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0){
        std::cerr << "listen error" << std::endl;
        exit(0);
    }
}

int Socket::accept(InetAddress* peerAddr)
{
    struct sockaddr_in addr;
    bzero(&addr,sizeof addr);
    socklen_t socklen = sizeof addr;
    int connfd = ::accept(sockfd, (struct sockaddr*)&addr, &socklen);
    if(connfd >= 0)
        peerAddr->setSockAddr(addr);
    return connfd;
}

void Socket::shutdownWrite()
{
    shutdown(sockfd, SHUT_WR);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}

int Socket::createNonblockingSocket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
            std::cerr << "socket error" << std::endl;
        exit(0);
    }
    setNonBlockAndCloseOnExec(sockfd);
    return sockfd;
}
