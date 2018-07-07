#ifndef HTTPCONTEXT_H_INCLUDED
#define HTTPCONTEXT_H_INCLUDED

#include "HttpRequest.h"

class Buffer;

class HttpContext
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine, kExpectHeaders, kExpectBody, kGetAll,
    };

    HttpContext() : request(), state(kExpectRequestLine) {};
    ~HttpContext() {};

    bool parseRequestLine() { return state == kExpectRequestLine; }
    bool parseHeaders() { return state == kExpectHeaders; }
    bool parseBody() { return state  == kExpectBody; }
    bool gotAll() {return state == kGetAll; }
    void receiveRequestLine() { state = kExpectHeaders; }
    void receiveHeaders() { state = kExpectBody; }
    void receiveBody() { state = kGetAll; }
    void reset() { state = kExpectRequestLine; request = HttpRequest(); }
    HttpRequest getRequest() { return request; }

    bool parseRequest(Buffer* buf);
    bool processRequestLine(string& line);


private:
    HttpRequest request;
    HttpRequestParseState state;
};


#endif // HTTPCONTEXT_H_INCLUDED
