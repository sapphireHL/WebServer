#include "Acceptor.h"
#include "unistd.h"

Acceptor::Acceptor(EventLoop* lp, InetAddress& listenAddr):
    loop(lp),
    acceptSocket(Socket::createNonblockingSocket()),
    acceptChannel(lp, acceptSocket.fd()),
    listening_(false)
{
    acceptSocket.setReuseAddr(true);
    acceptSocket.bindAddress(listenAddr);
    acceptChannel.setReadCallBack(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel.disableAll();
    acceptChannel.remove();
}

void Acceptor::listen()
{
    listening_ = true;
    acceptSocket.listen();
    acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket.accept(&peerAddr);
    if(connfd >= 0){
        if(newConnectionCallback)
            newConnectionCallback(connfd, peerAddr);
        else
            ::close(connfd);
    }
}
