#pragma once

#include <string>
#include <map>

class Client
{
    private:
        std::string _request;
        std::string _sendBuffer;
    public:
        Client();
        ~Client();
        int receiveFromClient( int fd, int epollFdCount );
        int sendToClient( int fd, const char *response );
        std::string getRequest();
};