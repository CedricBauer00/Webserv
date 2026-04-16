#include "../inc/Client.hpp"
#include "../inc/HttpParser.hpp"
#include <stdexcept>
#define BUFFER_SIZE 1024


Client::Client(int fd) : _fd(fd), _request(""), _sendBuffer( "sent" ), send_pos(0) 
{
    std::cout << BLUE << "Created client instance" << RESET << std::endl;
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
        if (0 < count)
            _request.append(buffer, static_cast<size_t>(count));
        else if (-1 < count) {
            std::cout << RED << "Client closed the connection\n" << RESET << std::endl;
            return 0;
        }
        else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
}

int Client::sendToClient(const char *response) {
    while (response[send_pos] != '\0') {
        ssize_t count = send(_fd, response + send_pos, strlen(response) - send_pos, 0);
        if (count == -1) {
            if (errno == EINTR)
                continue;
            std::cout << "Sent partial response (of len " 
            << send_pos << ") to fd=" << _fd << std::endl;
            return -1;
        }
        send_pos += count;
    }
    std::cout << "Sent complete response (of len "
    << send_pos << ") to fd=" << _fd << std::endl;
    return 0;
}

int Client::getSendPos() {
    return send_pos;
}

int Client::getFd() {
    return _fd;
}   