#pragma once

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
#include <vector>
#include "../inc/HttpParser.hpp"
#include "../inc/Client.hpp"
#include "../inc/ListenHandler.hpp"
#include "../inc/ConfigParsing.hpp"

#define PORT "3490"
#define BACKLOG 5

class Socket
{
    private:
        std::vector<int>    _listenFds;

    public:
        Socket();
        ~Socket();
        int createSocket( std::vector<Server> &servers );
        std::vector<int>    getListenFds();
};

int set_nonblocking(int fd);
void *get_in_addr(struct sockaddr *sa);
