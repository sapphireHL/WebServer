#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include <string.h>
#include <assert.h>

using namespace std;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* lp, std::string servername, InetAddress& listenAddr):
    loop(lp),
    name(servername),
    acceptor(new Acceptor(lp, listenAddr)),
    threadPool(new EventLoopThreadPool(loop)),
    started(false),
    nextConnId(1)
{
    acceptor->setNewConnectionCallback(bind(&TcpServer::newConnection,this, _1, _2));
    threadPool->setThreadNum(2);
}

TcpServer::~TcpServer()
{
    for(auto it = connections.begin(); it != connections.end(); ++it){
        TcpConnectionPtr conn = it->second;
        it->second.reset();
        conn->getLoop()->queueInLoop(bind(&TcpConnection::connectDestroyed, conn), conn->getFd());
        //conn->connectDestroyed();
        conn.reset();
    }
}

void TcpServer::newConnection(int sockfd, InetAddress& peerAddr)
{
    //printf("%zd connections\n", connections.size());
    //select a thread
    //printf("im loop %p , new connections come,transfering\n", loop);
    //EventLoop* ioLoop = threadPool->getNextLoop();
    string connName = name + "#" + to_string(nextConnId++);
    struct sockaddr_in localadr;
    bzero(&localadr, sizeof localadr);
    socklen_t len = static_cast<socklen_t>(sizeof(localadr));
    getsockname(sockfd, (struct sockaddr*)&localadr, &len);
    InetAddress localaddr(localadr);
    TcpConnectionPtr conn(new TcpConnection(loop, connName, sockfd, localaddr, peerAddr));
    //TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localaddr, peerAddr));
    connections[connName] = conn;
    conn->setConnectionCallback(connectionCallback);
    conn->setMessageCallback(messageCallback);
    conn->setCloseCallback(bind(&TcpServer::removeConnection, this, _1));
    conn->connectEstablished();
    //printf("loop:%p\n", ioLoop);
    //ioLoop->queueInLoop(bind(&TcpConnection::connectEstablished, conn), conn->getFd());
}

void TcpServer::start()
{
    //start thread pool
    //threadPool->start();
    //start listen
    acceptor->listen();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    //loop->queueInLoop(bind(&TcpServer::removeConnectionInLoop, this, conn), conn->getFd());
    //conn->getLoop()->queueInLoop(bind(&TcpConnection::connectDestroyed, conn));
    connections.erase(conn->getName());
    conn->connectDestroyed();
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    connections.erase(conn->getName());
    assert(conn->getLoop() != nullptr);
    conn->getLoop()->queueInLoop(bind(&TcpConnection::connectDestroyed, conn), conn->getFd());
}

void TcpServer::setThreadNum(int num)
{
    num = min(0, num);
    threadPool->setThreadNum(num);
}

