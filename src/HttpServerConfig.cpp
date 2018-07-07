#include "HttpServerConfig.h"
#include <fstream>

using namespace std;

HttpServerConfig::HttpServerConfig()
{
    //ctor
    initConfig();
}

HttpServerConfig::~HttpServerConfig()
{
    //dtor
}

void HttpServerConfig::initConfig()
{
    ifstream config("config", ios::in);
    if(config){
        string line;
        while(getline(config, line)){
            size_t space = line.find(' ');
            string key = line.substr(0, space);
            while(line[space] == ' ') ++space;
            string value = line.substr(space);
            if(key == "port"){
                port = stoi(value);
            }
            else if(key == "serverName"){
                serverName = value;
            }
            else if(key == "root"){
                rootPath = value;
            }
            else if(key == "threadNum"){
                threadNum = stoi(value);
            }
            else if(key == "allowPut"){
                allowPut = stoi(value);
            }
            else if(key == "allowDelete"){
                allowDelete = stoi(value);
            }
            else if(key == "showMessageReceived"){
                showMessageReceived = stoi(value);
            }
        }
    }
    config.close();
}
