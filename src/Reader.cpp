#include "../inc/Reader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Execution.hpp"
#include "../inc/Writer.hpp"

Reader::Reader(const Listener& listener)
    : AEventHandler(fd,
        listener.getServers(),
        listener.getEpoller(),
        EPOLLIN | EPOLLRDHUP | EPOLLET),
        _clientSockAddr(std::move(sockAddr)) {
}

Reader::~Reader() {
    std::cout << "FD " << _fd << ": [Reader] destroyed" << std::endl;
}

int	Reader::_acceptConn(int listenFd) {
	struct sockaddr_storage	sockAddr;
	socklen_t				addrLen{sizeof sockAddr};
	int						fd;

	fd = accept(
		listenFd, reinterpret_cast<struct sockaddr*>(&sockAddr), &addrLen);
	

}

int Reader::_receiveFromClient() {
     std::cout << BLUE << "FD " << _fd << ": [Reader] Reading from client.." 
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

Response	Reader::getResponse() const {
	if (!_complHeader)
		throw std::runtime_error(
            "[Reader] Header not complete, can't build response");
    return _buildResponse();
}

Response    Reader::_buildResponse() const {
    Response    res;
    Execution   e;

    e.execution(_request, res, _servers);
    return res;
}

void    Reader::_createWriter() {
    int writerFd = dup(_fd);

    if (writerFd == -1) {
        std::cerr << "FD " << _fd << ": [Reader] Error creating Writer, "
        << strerror(errno) << std::endl;
        return;
    }
    std::cout << "FD " << writerFd << ": [Reader] Writer FD created" << std::endl;
    try {
        new Writer(writerFd, *this);
    }
    catch (const std::exception& e) {
        std::cerr << "FD " << getFd() << ": [Reader] Error creating Writer, " 
        << e.what() << std::endl;
        closeFd(writerFd);
    }
}

void    Reader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
        int fd = getFd();
		_printSocketError();
		delete this;
		throw std::runtime_error(
            "FD " + std::to_string(fd) + ": Client disconnected unexpectedly");
    }

    if (_receiveFromClient() < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (!_complHeader)
				return; // No more data to read right now
			_createWriter();
		}
        else {
            delete this; // Will also remove from epoll
            throw std::runtime_error(
                std::string("recv: ") + strerror(errno));
        }
    }
    else {
		if (!_complHeader) {
            int fd = getFd();
			delete this; // Will also remove from epoll
			throw std::runtime_error(
                std::string("FD ") + std::to_string(fd) 
                + ": Client disconnected before completing header");
		}
        _createWriter();
    }
    delete this;
}