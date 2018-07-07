#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Socket.h"
#include "Channel.h"

class EventLoop;
class InetAddress;

class Acceptor
{
    public:
        typedef std::function<void(int sockfd, InetAddress&)> NewConnectionCallback;
        Acceptor(EventLoop* lp, InetAddress& listenAddr);
        ~Acceptor();

        void setNewConnectionCallback(const NewConnectionCallback& cb)
        {
            newConnectionCallback = cb;
        }

        bool listening() const{ return listening_; }
        void listen();

    private:
        void handleRead();

        EventLoop* loop; //
        Socket acceptSocket;
        Channel acceptChannel;
        NewConnectionCallback newConnectionCallback;
        bool listening_;
};

#endif // ACCEPTOR_H
