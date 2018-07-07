#include "HttpContext.h"
#include "Buffer.h"

using namespace std;

bool HttpContext::parseRequest(Buffer* buf)
{
    bool ok = true;
    bool hasMore = true;
    while(hasMore){
        if(parseRequestLine()){
            char* crlf = buf->findCRLF();
            if(crlf){
                string line(buf->data(), crlf);
                ok = processRequestLine(line);
                if(ok){
                    buf->readAsString(crlf + 2 - buf->data());
                    receiveRequestLine();
                }
                else{
                    hasMore = false;
                }
            }
            else{
                hasMore = false;
            }
        }
        else if(parseHeaders()){
            char* crlf = buf->findCRLF();
            if(crlf){
                string line(buf->data(), crlf);
                if(line == ""){
                    receiveHeaders();
                }
                else{
                    request.addHeader(line);
                }
                buf->readAsString(crlf + 2 - buf->data());
            }
            else{
                hasMore = false;
            }
        }
        else if(parseBody()){
            request.addBody(buf->retrieveAll());
            receiveBody();
            hasMore  = !gotAll();
        }
    }
    return ok;
}

bool HttpContext::processRequestLine(string& line)
{
    bool success = false;
    size_t space = line.find(' ');
    size_t start;
    if(space != string::npos && request.setMethod(line.substr(0, space))){
        start = space + 1;
        space = line.find(' ', start);
        if(space != string::npos){
            size_t question = line.find('?', start);
            if(question != string::npos){
                request.setPath(line.substr(start, question-start));
                request.setQuery(line.substr(question, space-question));
            }
            else{
                request.setPath(line.substr(start, space-start));
            }
            start = space + 1;
            string version = line.substr(start);
            if(version.size() == 8){
                if(version == "HTTP/1.1"){
                    success = true;
                    request.setVersion(HttpRequest::kHttp11);
                }
                else if(version == "HTTP/1.0"){
                    success = true;
                    request.setVersion(HttpRequest::kHttp10);
                }
            }
        }
    }
    return success;
}
