#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Configparsing/ConfigParser.hpp"

class AEventHandler
{
    protected:
        int _sockFd{-1};
        std::vector<const IWebservModule::SrvNode*>& _servers;
    public:
        AEventHandler(std::vector<const IWebservModule::SrvNode*>& servers);
        virtual ~AEventHandler();
        const int getSockFd() const;
        int setNonBlocking() const;
        virtual void process() = 0;
};
