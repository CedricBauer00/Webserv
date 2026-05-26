#include "../inc/Reader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"

Reader::Reader(const int fd, const Listener& listener)
    : AEventHandler(fd, listener.getServers(), listener.getEpoller()) {
    addSelfToEpoll(EPOLLIN | EPOLLRDHUP | EPOLLET);
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
        else if (-1 < count)
            return 0;
        else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
}

void    Reader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
		int err;
		socklen_t len = sizeof(err);

		getsockopt(_fd, SOL_SOCKET, SO_ERROR, &err, &len);
		std::cerr << "socket error: " << strerror(err) << std::endl;
		delete this;
		throw std::runtime_error("Client socket error");
    }

    if (receiveFromClient() < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return; // No more data to read right now
        else {
            delete this; // Will also remove from epoll
            throw std::runtime_error(
                std::string("recv: ") + strerror(errno));
        }
    }
    else {
		//TODO: if complete request has been received, it could be
		//that client side did a shutdown and still open for receiving
        delete this; // Will also remove from epoll
        throw std::runtime_error("Client disconnected");
    }
}