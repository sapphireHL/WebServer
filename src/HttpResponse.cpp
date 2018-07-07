#include "HttpResponse.h"
#include "Buffer.h"


void HttpResponse::appendToString(string& output)
{
    string responseLine = "HTTP/1.1 " + to_string(code) + statusMessage + "\r\n";
    output.append(responseLine);
    if(closeConnection){
        output.append("Connection: close\r\n");
    }
    else{
        output.append("Connection: Keep-Alive\r\n");
    }
    string contentLen = "Content-Length: " + to_string(body.size()) + "\r\n";
    output.append(contentLen);
    for(auto it = headers.begin(); it != headers.end(); ++it){
        output.append(it->first);
        output.append(": ");
        output.append(it->second);
        output.append("\r\n");
    }
    output.append("\r\n");
    output.append(body);
}
