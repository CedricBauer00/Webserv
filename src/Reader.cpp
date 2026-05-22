#include "../inc/Reader.hpp"

Reader::Reader(const int fd, const Listener& listener)
    : AEventHandler(fd, listener.getServers(), listener.getEpoller()) {
}

Reader::~Reader() {
}

int Reader::receiveFromClient() {
     std::cout << BLUE << "FD " << _fd << ": Reading from client.." 
     << RESET << std::endl;
    char buffer[BUFFER_SIZE];
        while (true) {
        ssize_t count = recv(_fd, buffer, sizeof(buffer), 0);
        if (0 < count)
        {
            _request.append(buffer, static_cast<size_t>(count));
            if( _request.find("\r\n\r\n") != std::string::npos )
                _complHeader = true;
        }
        else if (-1 < count) {
            std::cout << RED << "Client closed the connection\n" 
            << RESET << std::endl;
            return 0;
        }
        else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
}

void    Reader::process() {
    char buf[512];
    ssize_t bytesRead = recv(_fd, buf, sizeof(buf) - 1, 0);
    if (bytesRead == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Error reading from fd " << _fd << ": "
            << strerror(errno) << std::endl;
        }
        return;
    }
    else if (bytesRead == 0) {
        std::cout << "Client disconnected: fd=" << _fd << std::endl;
        return;
    }

    buf[bytesRead] = '\0';
    req += buf;
}