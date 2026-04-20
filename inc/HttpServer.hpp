#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <algorithm>
#include "../inc/HttpParser.hpp"
#include "../inc/Client.hpp"
#include "../inc/ListenHandler.hpp"
#include "../inc/ConfigParsing.hpp"

#define RED  "\033[31m"
#define ELEC_RED "\033[38;2;255;20;20m"
#define BLUE    "\033[34m"
#define GREEN  "\033[32m"
#define ORANGE  "\033[38;2;255;120;0m"
#define RESET  "\033[0m"

class HttpServer
{
    private:
        std::vector<int>    _listenFds;
        
    public:
        HttpServer();
        ~HttpServer();
        int createSocket( std::vector<Server> &servers );
        int eventLoop();
        void closeEvent(struct epoll_event &ev, int epollfd, int &epollFdCount);
        bool    getComplHeader();
};

#endif