#pragma once

#include <string>
#include <map>

class Client
{
    private:
        std::string _request;
        std::string _sendBuffer;
        int send_pos;
    public:
        Client();
        ~Client();
        int receiveFromClient( int fd );
        int sendToClient( int fd, const char *response );
        std::string getRequest();
        void closeFd( int fd, int epollfd, int &epoll_fd_count );
};