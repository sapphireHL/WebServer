#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <unordered_map>
using namespace std;

class Buffer;

class HttpResponse
{
    public:
        enum HttpStatusCode
        {
            kUnknown, k200OK = 200, k301MovedPermanently = 301, k302MovedTemporarily = 302,
            k400BadRequest = 400, k404NotFound = 404
        };

        HttpResponse(bool close = false): code(kUnknown), closeConnection(close){};
        ~HttpResponse() {};

        void addHeader(const string& field, const string& value) {headers[field] = value; }
        void setStatusCode(HttpStatusCode c) { code = c; }
        void setStatusMessage(const string& message) { statusMessage = message; }
        void setClose(bool close) { closeConnection = close; }
        bool isClose() { return closeConnection; }
        void setContentType(const string& type) { addHeader("Content-Type", type); }
        void setBody(const string& b) { body = b; }

        void appendToString(string& output);

    private:
        unordered_map<string, string> headers;
        HttpStatusCode code;
        bool closeConnection;
        string statusMessage;
        string body;
};

#endif // HTTPRESPONSE_H
