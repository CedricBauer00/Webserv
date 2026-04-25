#include "../inc/Client.hpp"
#include "../inc/HttpParser.hpp"
#include <stdexcept>
#define BUFFER_SIZE 1024


Client::Client(int fd) : EventHandler(fd), _request(""), _sendBuffer( "sent" ), _send_pos(0), _complHeader(false) 
{
    std::cout << BLUE << "Created client instance" << RESET << std::endl;
}

bool Client::getComplHeader()
{
    return _complHeader;
}

Client::~Client()
{
    std::cout << RED << "Destroyed client instance" << RESET << std::endl;
}

std::string Client::getRequest()
{
    return _request;
}

int Client::receiveFromClient()
{
    std::cout << BLUE << "client receives on FD = " << _fd << RESET << std::endl;
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t count = recv(_fd, buffer, sizeof(buffer), 0);
        if (count > 0 )
        {
            _request.append(buffer, static_cast<size_t>(count));
            if( _request.find("\r\n\r\n") != std::string::npos )
                _complHeader = true;
        }
        else if ( count == 0 )
        {
            if ( _request.empty() )
            {
                std::cout << RED << "Client closed the connection\n" << RESET << std::endl;
                return 0;
            }
            _complHeader = true;
            return 1;
        }
        else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
}

int Client::sendToClient(const char *response) {
    while (response[_send_pos] != '\0') {
        ssize_t count = send(_fd, response + _send_pos, strlen(response) - _send_pos, 0);
        if (count == -1) {
            if (errno == EINTR)
                continue;
            std::cout << "Sent partial response (of len " 
            << _send_pos << ") to fd=" << _fd << std::endl;
            return -1;
        }
        _send_pos += count;
    }
    std::cout << "Sent complete response (of len "
    << _send_pos << ") to fd=" << _fd << std::endl;
    return 0;
}

int Client::getSendPos() {
    return _send_pos;
}
