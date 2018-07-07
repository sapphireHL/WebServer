#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpConnection.h"
#include "TypeDefines.h"
#include "InetAddress.h"
#include <map>
#include <memory>

class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpServer
{
    public:
        TcpServer(EventLoop* lp, std::string servername, InetAddress& listenAddr);
        ~TcpServer();

        void start();//start server

        void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback = cb; }
        void setMessageCallback(const MessageCallback& cb) { messageCallback = cb; }
        void setThreadNum(int num);

    private:
        void newConnection(int sockfd, InetAddress& peerAddr);
        void removeConnection(const TcpConnectionPtr& conn);
        void removeConnectionInLoop(const TcpConnectionPtr& conn);

        typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

        EventLoop* loop;
        std::string name;
        std::unique_ptr<Acceptor> acceptor;
        std::shared_ptr<EventLoopThreadPool> threadPool;
        ConnectionCallback connectionCallback;
        MessageCallback messageCallback;
        bool started;
        int nextConnId;
        ConnectionMap connections;
};

#endif // TCPSERVER_H
