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
#include <signal.h>

class httpServer
{
    private:

    public:
        httpServer();
        ~httpServer();
        int run();
};

#endif