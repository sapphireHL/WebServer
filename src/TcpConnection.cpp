#include "TcpConnection.h"
#include "Socket.h"
#include "Channel.h"
#include <unistd.h>
#include <assert.h>

using namespace std;
using namespace std::placeholders;

TcpConnection::TcpConnection(EventLoop* lp, std::string n, int sockfd,
                             InetAddress localAdr, InetAddress peerAdr):
                          loop(lp),
                          name(n),
                          state(kConnecting),
                          socket(new Socket(sockfd)),
                          channel(new Channel(loop, sockfd)),
                          localAddr(localAdr),
                          peerAddr(peerAdr),
                          inputBuffer(),
                          outputBuffer()

{
    channel->setReadCallBack(bind(&TcpConnection::handleRead, this));
    //channel->setWriteCallBack(bind(&TcpConnection::handleWrite, this));
    //channel->setCloseCallBack(bind(&TcpConnection::handleClose, this));
    //channel->setErrorCallBack(bind(&TcpConnection::handleError, this));
    socket->setKeepAlive(true);
    socket->setReuseAddr(true);
}

TcpConnection::~TcpConnection()
{
    //dtor
    assert(state == kDisconnected);
}

void TcpConnection::handleRead()
{
    //char buf[65536] ={0};
    size_t n = inputBuffer.readFd(channel->fd());
    //size_t n = read(channel->fd(), buf, 65536);
    if(n > 0)
        messageCallback(shared_from_this(), &inputBuffer, n);
    else if (n == 0)
        handleClose();
    else
        handleError();
}

void TcpConnection::handleClose()
{
    if(state == kConnected  || state == kDisConnecting){
        //setState(kDisconnected);
        channel->disableAll();
        //connectionCallback(shared_from_this());

        closeCallback(shared_from_this());
    }

}

void TcpConnection::handleError()
{

}

void TcpConnection::handleWrite()
{
    if(channel->isWriting()){
        ssize_t n = write(channel->fd(), outputBuffer.data(), outputBuffer.readableBytes());
        if(n > 0){
            //delete n bytes
            outputBuffer.readAsString(n);
            if(outputBuffer.readableBytes() == 0){
                //send complete, finish writing
                channel->disableWriting();
                if(state == kDisConnecting)
                    shutdown();
            }
        }
    }
}

void TcpConnection::connectEstablished()
{
    //printf("establish\n");
    setState(kConnected);
    channel->enableReading();
    connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    //printf("close\n");
    if(state == kConnected || state == kDisConnecting){
        setState(kDisconnected);
        channel->disableAll();
        connectionCallback(shared_from_this());
        channel->remove();
        //loop->removeChannel(channel);
    }
}

void TcpConnection::shutdown()
{
    //printf("close\n");
    if(state == kConnected)
        setState(kDisConnecting);
    if(!channel->isWriting()){
        channel->disableAll();
        socket->shutdownWrite();
        closeCallback(shared_from_this());
    }
}

void TcpConnection::send(string data)
{
    ssize_t n = 0;
    if(state == kConnected)
    {
        if(!channel->isWriting() && outputBuffer.readableBytes() == 0){
            n = write(channel->fd(), data.data(), data.size());
            if(n >= 0){
                if((size_t)n < data.size()){
                    //going to write more data
                }
            }
            else{
                n = 0;
            }
        }
        if((size_t)n < data.size()){
            outputBuffer.append(data.data()+n, data.size()-n);
            if(!channel->isWriting())
                channel->enableWriting();
        }
    }
}
