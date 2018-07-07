#ifndef HTTPSERVERCONFIG_H
#define HTTPSERVERCONFIG_H

#include <string>

class HttpServerConfig
{
    public:
        HttpServerConfig();
        ~HttpServerConfig();

        uint16_t getPort() const { return port; }
        std::string getServerName() const { return serverName; }
        std::string getRootPath() const { return rootPath; }
        int getThreadNum() const { return threadNum; }
        bool getAllowPut() const { return allowPut; }
        bool getAllowDelete() const { return allowDelete; }
        bool getShowMessageReceived() const { return showMessageReceived; }

    private:
        void initConfig();

        uint16_t port;
        std::string serverName;
        std::string rootPath;
        int threadNum;
        bool allowPut;
        bool allowDelete;
        bool showMessageReceived;
};

#endif // HTTPSERVERCONFIG_H
