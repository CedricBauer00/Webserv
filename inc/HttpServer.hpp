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

#define RED  "\033[31m"
#define ELEC_RED "\033[38;2;255;20;20m"
#define BLUE    "\033[34m"
#define GREEN  "\033[32m"
#define ORANGE  "\033[38;2;255;120;0m"
#define RESET  "\033[0m"

class HttpServer
{
    private:
        int _listenSock;
    public:
        HttpServer();
        ~HttpServer();
        int createSocket();
        int eventLoop();
        int get_sock();
        void closeEvent(int fd, int epollfd, int &epollFdCount);
};

#endif