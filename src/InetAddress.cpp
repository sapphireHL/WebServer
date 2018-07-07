#include "InetAddress.h"
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

static const in_addr_t kInaddrAny = INADDR_ANY;//可以绑定任何网卡
static const in_addr_t kInaddrLoopBack = INADDR_LOOPBACK;//本地环路测试

InetAddress::InetAddress(uint16_t port, bool loopbackonly)
{
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    in_addr_t ip = loopbackonly ? kInaddrLoopBack : kInaddrAny;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_port = htons(port);
}

InetAddress::InetAddress(std::string ip, uint16_t port)
{
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress()
{
    //dtor
}

string InetAddress::toIp() const
{
    char* buf = inet_ntoa(addr.sin_addr);
    return string(buf);
}

string InetAddress::toIpPort() const
{
    char* buf = inet_ntoa(addr.sin_addr);
    uint16_t port = ntohs(addr.sin_port);
    return string(buf) + ":" + to_string(port);
}

uint16_t InetAddress::toPort() const
{
    uint16_t port = ntohs(addr.sin_port);
    return port;
}
