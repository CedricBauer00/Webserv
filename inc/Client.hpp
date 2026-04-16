#pragma once

#include <string>
#include <map>

class Client
{
    private:
        int _fd;
        std::string _request;
        std::string _sendBuffer;
        int send_pos;
    public:
        Client(int fd);
        ~Client();
        int receiveFromClient();
        int sendToClient(const char *response );
        std::string getRequest();
        int getSendPos();
        int getFd();
};