#include "../inc/Client.hpp"
#include "../inc/Httpparser.hpp"
#include <stdexcept>
#define BUFFER_SIZE 1024


Client::Client() : _request( "Received" ), _sendBuffer( "sent" ), send_pos(0)
{
    std::cout << BLUE << "handling client" << RESET << std::endl;
}

Client::~Client()
{
    std::cout << RED << "Destroyed client instance" << RESET << std::endl;
}

std::string Client::getRequest()
{
    return _request;
}

void Client::closeFd(int fd, int epollfd, int &epollFdCount )
{
    if ( epoll_ctl( epollfd, EPOLL_CTL_DEL, fd, NULL ) == -1 )
    {
        perror( "epoll_ctl:delete fd" );
        throw std::runtime_error("EPOLL_CTL_ERROR");
    }
    if ( close( fd ) == -1)
        throw std::runtime_error("CLOSE_ERROR");
    std::cout << RED << "Closed: fd=" << fd << RESET << std::endl;
    --epollFdCount;
}

int Client::receiveFromClient( int fd )
{
    std::cout << BLUE << "client receives on FD = " << fd << RESET << std::endl;
    
    char buffer[BUFFER_SIZE];
    while ( true )
    {
        ssize_t count = recv( fd, buffer, sizeof(buffer), 0 );
        std::cout << "count:" << count << std::endl;
        if ( count > 0 )
        {
            _request.append( buffer, static_cast<size_t>( count ) );
        }
        else if (count == 0)
        {
            std::cout << RED << "Client closed the connection\n" << RESET << std::endl;
            return 0;
        }
        else
        {
            std::cout << " request: " << _request << std::endl;
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
}

int Client::sendToClient( int fd, const char *response )
{
    std::cout << "Sent response to fd=" << fd << std::endl;

    while (response[send_pos] != '\0')
    {
        ssize_t count = send( fd, response, strlen( response ), 0 );
        if ( count == -1 )
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        send_pos += count;
    }
    return 0;
}