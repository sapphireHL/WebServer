#ifndef TYPEDEFINES_H_INCLUDED
#define TYPEDEFINES_H_INCLUDED

#include <functional>
#include <memory>
#include <Timestamp.h>

class TcpConnection;
class Buffer;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)>  ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)>  CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*, size_t len)> MessageCallback;


#endif // TYPEDEFINES_H_INCLUDED
