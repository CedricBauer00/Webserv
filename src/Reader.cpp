#include "../inc/Reader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"

Reader::Reader(const int fd, const Listener& listener)
    : AEventHandler(fd,
        listener.getServers(),
        listener.getEpoller(),
        EPOLLIN | EPOLLRDHUP | EPOLLET) {
}

Reader::~Reader() {
}

int Reader::_receiveFromClient() {
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
		_printSocketError();
		delete this;
		throw;
    }

    if (receiveFromClient() < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (!_complHeader)
				return; // No more data to read right now
			//TODO: run stuff
		}
        else {
            delete this; // Will also remove from epoll
            throw std::runtime_error(
                std::string("recv: ") + strerror(errno));
        }
    }
    else {
		if (!_complHeader) {
			std::cerr << "FD " << _fd << ": Client disconnected" << std::endl;
			delete this; // Will also remove from epoll
			throw;
		}
		//TODO: run stuff
    }
}