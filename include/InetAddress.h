#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h>
#include<string>

class InetAddress
{
    public:
        explicit InetAddress(uint16_t port = 0, bool loopbackonly = false);
        InetAddress(std::string ip, uint16_t port);
        InetAddress(const struct sockaddr_in& add) : addr(add) {};

        std::string toIp() const;
        std::string toIpPort() const;
        uint16_t toPort() const;

        struct sockaddr_in getSockAddr() { return addr; };
        void setSockAddr(const struct sockaddr_in& add) { addr = add; };

        // 返回网络字节序的ip地址
        uint32_t ipNetEndian() const { return addr.sin_addr.s_addr; }
        // 返回网络字节序的port
        uint16_t portNetEndian() const { return addr.sin_port; }

        ~InetAddress();


    private:
        struct sockaddr_in addr;
};

#endif // INETADDRESS_H
