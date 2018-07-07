#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "TcpServer.h"
#include "RedisClient.h"
#include "EventLoop.h"
#include "HttpServerConfig.h"
#include "ThreadPool.h"
#include <unordered_map>
#include <memory>
#include <cpp_redis/cpp_redis>

class HttpRequest;
class HttpResponse;
class HttpContext;
class Buffer;

class HttpServer
{
    public:
        typedef std::function<void(const HttpRequest&, HttpResponse*)> HttpCallback;
        typedef std::shared_ptr<HttpContext> HttpContextPtr;

        HttpServer();
        ~HttpServer();
        //void setHttpCallback(const HttpCallback& cb) { httpCallback = cb; }

        void start();
        std::string getRootPath()  {return rootPath;}
        void processMessage(HttpContextPtr& context, const TcpConnectionPtr& conn, Buffer* data);

    private:
        void onConnection(const TcpConnectionPtr& conn);
        void onMessage(const TcpConnectionPtr& conn, Buffer* data, size_t n);
        void onRequest(const TcpConnectionPtr& conn, const HttpRequest& request);

        void handleGet(const HttpRequest& request, HttpResponse* response);
        void handlePost(const HttpRequest& request, HttpResponse* response);
        void handleHead(const HttpRequest& request, HttpResponse* response);
        void handlePut(const HttpRequest& request, HttpResponse* response);
        void handleDelete(const HttpRequest& request, HttpResponse* response);
        void handleBad(HttpResponse* response);
        void handleHttpRequest(const HttpRequest& request, HttpResponse* response);

        //special html
        void handlePostToSignupHtml(const HttpRequest& request, HttpResponse* response);
        void handlePostToIndexHtml(const HttpRequest& request, HttpResponse* response);
        void handlePostToHomeHtml(const HttpRequest& request, HttpResponse* response);
        void fillHomeHtml(const HttpRequest& request, std::string& body);
        void handlePostToHomeHtmlUpload(const HttpRequest& request, HttpResponse* response);
        void handlePostToHomeHtmlDelete(const HttpRequest& request, HttpResponse* response);

        std::vector<std::pair<std::string, std::string>> extractFromBody(const std::string& body);
        std::string getFile(const std::string & filePath, const std::string& append="");

        HttpServerConfig config;
        EventLoop loop;
        InetAddress listenAddr;
        TcpServer server;
        int threadNum;
        bool allowPut;
        bool allowDelete;
        bool showMessageReceived;
        //HttpCallback httpCallback;
        std::unordered_map<std::string, HttpContextPtr> contexts;
        std::string rootPath;
        RedisClient redisClient;
        ThreadPool threadPool;
};

#endif // HTTPSERVER_H
