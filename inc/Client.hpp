#pragma once

#include <string>
#include <map>
#include "EventHandler.hpp"

class Client: public EventHandler
{
    private:
        std::string _request;
        std::string _sendBuffer;
        int         _send_pos;
        bool        _complHeader;
    public:
        Client(int fd);
        ~Client();
        int receiveFromClient();
        int sendToClient( std::string Response );
        std::string getRequest();
        int getSendPos();
        bool getComplHeader();
};