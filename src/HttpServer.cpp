#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpContext.h"
#include "HttpResponse.h"
#include "Buffer.h"
#include <iostream>
#include <fstream>
#include <future>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

using namespace std;
using namespace std::placeholders;

vector<string> getFilesInFolder(const string& path)
{
    vector<string> res;
    DIR* dir;
    struct dirent * ptr;
    if((dir = opendir(path.c_str())) == nullptr){
        cerr << "open dir error" << endl;
    }
    while((ptr = readdir(dir)) != nullptr){
        if(strcmp(ptr->d_name, ".")==0 || strcmp(ptr->d_name, "..")==0)
            continue;
        //file
        if(ptr->d_type == DT_REG){
            res.push_back(ptr->d_name);
        }
        //dir
        //not support
        else if(ptr->d_type == DT_DIR){
            vector<string> subpath = getFilesInFolder(ptr->d_name);
            for(string s:subpath)
                res.push_back(s);
        }
    }
    sort(res.begin(), res.end());
    closedir(dir);
    return res;
}

vector<pair<string, string>> HttpServer::extractFromBody(const string& body)
{
    vector<pair<string, string>> res;
    size_t start = 0, seperation;
    while((seperation = body.find('&', start)) != string::npos){
        size_t equal = body.find('=', start);
        string key = body.substr(start, equal);
        string value = body.substr(equal+1, seperation-equal-1);
        res.push_back(make_pair(key, value));
        start = seperation + 1;
    }
    size_t equal = body.find('=', start);
    string key = body.substr(start, equal);
    string value = body.substr(equal+1);
    res.push_back(make_pair(key, value));
    return res;
}

string HttpServer::getFile(const string& filePath, const string & append)
{
    string content, line;
    ifstream in(filePath, ios::in);
    if(in){
        while(getline(in, line)){
            content += line;
            content += "\r\n";
        }
    }
    in.close();
    content += append;
    return content;
}

void setNotFound(HttpResponse* response)
{
    string body = "<html><head><title>hello</title></head><body><p>file not found</p></body></html>";
    response->setStatusCode(HttpResponse::k404NotFound);
    response->setStatusMessage("NotFound");
    response->setClose(true);
    response->setBody(body);
    response->setContentType("text/html");
}

void HttpServer::fillHomeHtml(const HttpRequest& request, string& body)
{
    string line;
    bool userModified = false;
    string user = request.getHeader("Cookie");
    user = user.substr(user.find('=') + 1);
    string userPath = rootPath + "/" + user + "/";
    ifstream in(rootPath + request.getPath(), ios::in);
    if(in){
        while(getline(in, line)){
            if(!userModified){
                    size_t pos;
                if((pos = line.find("xxx")) != string::npos){
                    line.replace(pos, 3, user);
                    userModified = true;
                }
            }
            body += line;
            //show all files in user path, download available,only surpported by chrome and firefox
            if(line == "<fieldset>"){
                vector<string> files = getFilesInFolder(userPath);
                for(int i = 0; i < (int)files.size(); ++i){
                    string relativePath = "/" + user + "/" + files[i];
                    body += "<center><a href=\"" + relativePath + "\" download=\"" + files[i] + "\"" + ">";
                    body += files[i];
                    body += "</a>";
                    body += "&nbsp";//space
                    body += "<input type = \"submit\" name = \"" + files[i] + "\" value = \"delete\">";
                    body += "</center>";
                }
            }
        }
    }
    in.close();
}

void HttpServer::handleGet(const HttpRequest& request, HttpResponse* response)
{
    string filePath = getRootPath() + request.getPath();
    //can not access without login
    if(request.getPath() == "/home.html"){
        string referer = request.getHeader("Referer");
        if(referer == "http://127.0.0.1:" + to_string(config.getPort()) + "/index.html"){
            string body;
            fillHomeHtml(request, body);
            if(body.size()){
                response->setBody(body);
                response->setStatusCode(HttpResponse::k200OK);
                response->setStatusMessage("OK");
                response->setContentType("text/html");
            }
            else{
                setNotFound(response);
            }
        }
        else{
            response->setStatusCode(HttpResponse::k302MovedTemporarily);
            response->setStatusMessage("Moved Temporarily");
            response->addHeader("Location", "index.html");
        }
    }
    else{
        string body = getFile(filePath);
        if(body.size()){
            response->setBody(body);
            response->setStatusCode(HttpResponse::k200OK);
            response->setStatusMessage("OK");
            response->setContentType("text/html");
        }
        else{
            setNotFound(response);
        }
    }
}

void HttpServer::handlePost(const HttpRequest& request, HttpResponse* response)
{
    string filePath = getRootPath() + request.getPath();
    fstream in(filePath, ios::in);
    if(in){
        //login submit
        if(request.getPath() == "/index.html"){
            handlePostToIndexHtml(request, response);
        }
        //sign up submit
        else if(request.getPath() == "/signup.html"){
            handlePostToSignupHtml(request, response);
        }
        //home page
        else if(request.getPath() == "/home.html"){
            handlePostToHomeHtml(request, response);
        }
    }
    else{
        setNotFound(response);
    }
    in.close();
}

void HttpServer::handlePut(const HttpRequest& request, HttpResponse* response)
{
    if(allowPut){

    }
}

void HttpServer::handleDelete(const HttpRequest& request, HttpResponse* response)
{
    if(allowDelete){

    }
}

void HttpServer::handleHead(const HttpRequest& request, HttpResponse* response)
{
    string filePath = getRootPath() + request.getPath();
    ifstream in(filePath, ios::in);
    if(in){
        response->setStatusCode(HttpResponse::k200OK);
        response->setStatusMessage("OK");
    }
    else{
        response->setStatusCode(HttpResponse::k404NotFound);
        response->setStatusMessage("NotFound");
        response->setClose(true);
    }
    in.close();
}

void HttpServer::handleBad(HttpResponse* response)
{
    response->setStatusCode(HttpResponse::k400BadRequest);
    response->setStatusMessage("Bad Request");
    response->setClose(true);
}

void HttpServer::handlePostToIndexHtml(const HttpRequest& request, HttpResponse* response)
{
    string body = request.getBody();
    vector<pair<string, string>> paras = extractFromBody(body);
    string user = paras[0].second;
    string pwd = paras[1].second;
    string realpwd = redisClient.redisHget(user, "password");
    //cout<<"pwd:"<<realpwd<<endl;
    if(pwd == realpwd){
        //302 to home page
        response->setStatusCode(HttpResponse::k302MovedTemporarily);
        response->setStatusMessage("Moved Temporarily");
        response->addHeader("Location", "home.html");
        response->addHeader("Set-Cookie", "user=" + user);
        //make file folder
        string userPath = rootPath + "/" + user;
        mkdir(userPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR );
    }
    else{
        string indexPath = rootPath + "/index.html";
        string appendDialog = "<script language=\"javascript\">"
        "alert(\"username or password error!\");</script> ";
        string body = getFile(indexPath);
        if(body.size()){
            body += appendDialog;
            response->setBody(body);
            response->setStatusCode(HttpResponse::k200OK);
            response->setStatusMessage("OK");
            response->setContentType("text/html");
        }
        else{
            setNotFound(response);
        }
    }
}

void HttpServer::handlePostToSignupHtml(const HttpRequest& request, HttpResponse* response)
{
    string body = request.getBody();
    vector<pair<string, string>> paras = extractFromBody(body);
    string user = paras[0].second;
    string pwd = paras[1].second;
    string confirm = paras[2].second;
    bool existUser = redisClient.redisExist(user);
    if(!existUser){
        //new user
        if(pwd == confirm){
            redisClient.redisHset(user, "password", pwd);
            redisClient.redisBgsave();
            //302 to index page
            response->setStatusCode(HttpResponse::k302MovedTemporarily);
            response->setStatusMessage("Moved Temporarily");
            response->addHeader("Location", "index.html");

            string appendDialog = "<script language=\"javascript\">"
            "alert(\"new account has been created! \");</script> ";
        }
        else{
            string signupPath = rootPath + "/signup.html";
            string appendDialog = "<script language=\"javascript\">"
            "alert(\"password and confirm are not same! \");</script> ";
            string body = getFile(signupPath);
            if(body.size()){
                body += appendDialog;
                response->setBody(body);
                response->setStatusCode(HttpResponse::k200OK);
                response->setStatusMessage("OK");
                response->setContentType("text/html");
            }
            else{
                setNotFound(response);
            }
        }
    }
    else{
        //already exist
        string signupPath = rootPath + "/signup.html";
        string appendDialog = "<script language=\"javascript\">"
        "alert(\"username already exists!\");</script> ";
        string body = getFile(signupPath);
        if(body.size()){
            body += appendDialog;
            response->setBody(body);
            response->setStatusCode(HttpResponse::k200OK);
            response->setStatusMessage("OK");
            response->setContentType("text/html");
        }
        else{
            setNotFound(response);
        }
    }
}

void HttpServer::handlePostToHomeHtml(const HttpRequest& request, HttpResponse* response)
{
    string contentType = request.getHeader("Content-Type");
    //upload
    if(contentType.find("multipart/form-data") != string::npos){
        handlePostToHomeHtmlUpload(request, response);
    }
    //delete
    else{
        handlePostToHomeHtmlDelete(request, response);
    }
}

void HttpServer::handlePostToHomeHtmlUpload(const HttpRequest& request, HttpResponse* response)
{
    //userpath to upload file
    string user = request.getHeader("Cookie");
    user = user.substr(user.find('=') + 1);
    string userPath = rootPath + "/" + user + "/";
    //get boundary
    string boundary = request.getHeader("Content-Type");
    boundary = boundary.substr(boundary.find('=') + 1);
    string bodyBoundary = "--" + boundary;
    string endBoundary = bodyBoundary + "--";
    //get body
    string body = request.getBody();
    //get one line from body
    size_t seperator, prev = 0;
    string line, filename, filecontent;
    bool hasFileName = false, hasFileContent = false;
    while((seperator = body.find("\r\n", prev)) != string::npos){
        line = body.substr(prev, seperator-prev);
        seperator += 2;
        prev = seperator;
        //end of body
        if(line == endBoundary){
            hasFileContent = false;
        }
        //find filename
        if(hasFileName){
            size_t filenamePos = line.find("filename") + 10;
            size_t filenamePosEnd = line.find('\"', filenamePos);
            filename = line.substr(filenamePos, filenamePosEnd - filenamePos);
            hasFileName = false;
        }
        //append content
        else if(hasFileContent){
            filecontent += line;
            filecontent += "\r\n";
        }
        //next line has filename
        if(line == bodyBoundary){
            hasFileName = true;
        }
        //next line has content
        else if(line == ""){
            hasFileContent = true;
        }
    }
    //write to file
    string fullPath = userPath + filename;
    ofstream out(fullPath, ios::out);
    if(out)
        out << filecontent;
    out.close();
    //update page
    string newbody;
    fillHomeHtml(request, newbody);
    response->setBody(newbody);
    response->setStatusCode(HttpResponse::k200OK);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
}

void HttpServer::handlePostToHomeHtmlDelete(const HttpRequest& request, HttpResponse* response)
{
    string body = request.getBody();
    size_t equal = body.find('=');
    string filenameToDelete = body.substr(0, equal);
    string user = request.getHeader("Cookie");
    user = user.substr(user.find('=') + 1);
    string userPath = rootPath + "/" + user + "/";
    string deletePath = userPath + filenameToDelete;
    //delete file
    remove(deletePath.c_str());
    //update page
    string newbody;
    fillHomeHtml(request, newbody);
    response->setBody(newbody);
    response->setStatusCode(HttpResponse::k200OK);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
}

void HttpServer::handleHttpRequest(const HttpRequest& request, HttpResponse* response)
{
    HttpRequest::Method method = request.getMethod();
    switch(method)
    {
    case HttpRequest::kGet:
        handleGet(request, response);
        break;
    case HttpRequest::kPost:
        handlePost(request, response);
        break;
    case HttpRequest::kPut:
        handlePut(request, response);
        break;
    case HttpRequest::kDelete:
        handleDelete(request, response);
        break;
    case HttpRequest::kHead:
        handleHead(request, response);
        break;
    default:
        handleBad(response);
        break;
    };
}

HttpServer::HttpServer():
    config(),
    loop(),
    listenAddr(config.getPort()),
    server(&loop, config.getServerName(), listenAddr),
    threadNum(config.getThreadNum()),
    allowPut(config.getAllowPut()),
    allowDelete(config.getAllowDelete()),
    showMessageReceived(config.getShowMessageReceived()),
    rootPath(config.getRootPath()),
    redisClient(),
    threadPool()
{
    //server.setThreadNum(config.getThreadNum());
    server.setConnectionCallback(bind(&HttpServer::onConnection,this, _1));
    server.setMessageCallback(bind(&HttpServer::onMessage, this, _1,_2, _3));
    cout << "listening on port:" << config.getPort() << endl;
}

HttpServer::~HttpServer()
{
    //dtor
    //cout<<"bye bye"<<endl;
}

void HttpServer::start()
{
    //threadPool.start(2);
    server.start();
    loop.loop();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected()){
        contexts[conn->getName()] = make_shared<HttpContext>();
        //cout<<"establish\n";
    }
    else{
        if(contexts.count(conn->getName())){
            auto it = contexts.find(conn->getName());
            //delete it->second;
            contexts.erase(it);
        }
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* data, size_t n)
{
    HttpContextPtr context = contexts[conn->getName()];
    //multi thread
    //threadPool.addTask(bind(&HttpServer::processMessage, this, context, conn, data));
    processMessage(context, conn, data);
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& request)
{
    const string connection = request.getHeader("Connection");
    bool close = (connection == "close") || (request.getVersion() == HttpRequest::kHttp10);
    HttpResponse response(close);
    handleHttpRequest(request, &response);
    string buf;
    response.appendToString(buf);
    conn->send(buf);
    //cout<<"send "<<buf.size()<<" bytes\n";
    if(response.isClose()){
        conn->shutdown();
        //server.removeConnection(conn);
    }
}

void HttpServer::processMessage(HttpContextPtr& context, const TcpConnectionPtr& conn, Buffer* data)
{
    if(context){
        if(showMessageReceived){
            string message(data->data(), data->readableBytes());
            cout<<message<<endl;
        }
        if(!context->parseRequest(data)){
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
            //server.removeConnection(conn);
        }
        else{
            if(context->gotAll()){
                onRequest(conn, context->getRequest());
                context->reset();
            }
        }
    }
}
