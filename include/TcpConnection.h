#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "TypeDefines.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "Socket.h"
#include <memory>

class EventLoop;
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
    public:
        TcpConnection(EventLoop* lp, std::string name, int sockfd,
                      InetAddress localAdr, InetAddress peerAdr);
        ~TcpConnection();

        void connectEstablished();
        void connectDestroyed();
        void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback =  cb; }
        void setMessageCallback(const MessageCallback& cb) { messageCallback =  cb; }
        void setCloseCallback(const CloseCallback& cb) { closeCallback =  cb; }

        bool connected() { return state == kConnected; }
        InetAddress getPeerAddr() { return peerAddr; }
        std::string getName() { return name; }
        EventLoop* getLoop() { return loop; }
        int getFd() { return socket->fd(); }

        void send(std::string data);
        void shutdown();


    private:
        enum State
        {
            kConnecting, kConnected, kDisConnecting, kDisconnected,
        };

        void setState(State s) { state = s; };
        void handleRead();
        void handleWrite();
        void handleError();
        void handleClose();

        EventLoop* loop;
        std::string name;
        State state;
        std::unique_ptr<Socket> socket;
        std::unique_ptr<Channel> channel;
        InetAddress localAddr;
        InetAddress peerAddr;
        ConnectionCallback connectionCallback;
        MessageCallback messageCallback;
        CloseCallback closeCallback;
        Buffer inputBuffer;
        Buffer outputBuffer;
};

#endif // TCPCONNECTION_H
