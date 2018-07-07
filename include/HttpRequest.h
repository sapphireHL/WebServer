#ifndef HTTPREQUEST_H_INCLUDED
#define HTTPREQUEST_H_INCLUDED

#include <unordered_map>
using namespace std;

class HttpRequest
{
public:
    enum Method
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version
    {
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest() : method(kInvalid), version(kUnknown) {};
    ~HttpRequest(){};

    void setVersion(Version v) { version = v; }
    Version getVersion() const { return version; }

    bool setMethod(string m)
    {
        if(m == "GET") method = kGet;
        else if(m == "POST") method = kPost;
        else if(m == "HEAD") method = kHead;
        else if(m == "PUT") method = kPut;
        else if(m == "DELETE") method = kDelete;
        else method = kInvalid;
        return method != kInvalid;
    }
    Method getMethod() const { return method; }
    string methodString()
    {
        string res;
        switch(method)
        {
        case kInvalid:
            res = "INVALID";
            break;
        case kGet:
            res = "GET";
            break;
        case kPost:
            res = "POST";
            break;
        case kHead:
            res = "HEAD";
            break;
        case kPut:
            res = "PUT";
            break;
        case kDelete:
            res = "DELETE";
            break;
        default:
            break;
        };
        return res;
    }

    void addHeader(string& line)
    {
        int posColon = line.find(":");
        string field = line.substr(0, posColon);
        while((size_t)++posColon < line.size() && line[posColon] == ' ') {};
        string value = line.substr(posColon);
        headers[field] = value;
    }
    string getHeader(const string& field) const
    {
        if(headers.count(field))
        {
            unordered_map<string, string>::const_iterator it = headers.find(field);
            return it->second;
        }
        else
            return string();
    }
    const unordered_map<string, string>& getHeaders() { return headers; }
    void setPath(const string& p){ path = p; }
    string getPath() const { return path; }
    void setQuery(const string& q){ query = q; }
    string getQuery() const { return query; }
    void addBody(const string& b) { body += b; };
    string getBody() const { return body; }

private:
    Method method;
    Version version;
    string path;
    string query;
    string body;
    unordered_map<string, string> headers;
};

#endif // HTTPREQUEST_H_INCLUDED
