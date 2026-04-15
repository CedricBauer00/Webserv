#include "../inc/Client.hpp"
#include "../inc/Httpparser.hpp"

#define BUFFER_SIZE 1024


Client::Client() : _request( "Received" ), _sendBuffer( "sent" )
{
    std::cout << "handling client" << std::endl;
}

Client::~Client()
{
    std::cout << "Destroyed client instance" << std::endl;
}

std::string Client::getRequest()
{
    return _request;
}

int Client::receiveFromClient( int fd, int epollFdCount )
{
    std::cout << "client receives" << std::endl;
    
    char buffer[BUFFER_SIZE];
    while ( true )
    {
        ssize_t count = recv( fd, buffer, sizeof( buffer ), 0 );

        if ( count > 0 )
            _request.append( buffer, static_cast<size_t>( count ) );
        else if (count == 0)
        {
            std::cout << RED << "Client closed the connection\n" << RESET << std::endl;
            break;
        }
        else
        {
            if ( errno == EAGAIN || errno == EWOULDBLOCK )
                break;
            if ( errno == EINTR )
                continue;
            perror( "recv" );
            if ( close( fd ) == -1 )
                return -1;
            std::cout << RED << "Closed: fd=" << fd << RESET << std::endl;
            --epollFdCount;
            break;
        }
    }
    return 0;
}

int Client::sendToClient( int fd, const char *response )
{
    if ( send( fd, response, strlen( response ), 0 ) == -1 )
        perror( "send" );
    std::cout << "Sent response to fd=" << fd << std::endl;
    
    return 0;
}