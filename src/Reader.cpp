#include "../inc/Reader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Writer.hpp"

Reader::Reader(const Listener& listener)
    : AEventHandler(_acceptConn(listener.getFd()),
        listener.getServers(),
        listener.getEpoller(),
        EPOLLIN | EPOLLRDHUP | EPOLLET) {
}

Reader::~Reader() {
    std::cout << "FD " << _fd << ": [Reader] destroyed" << std::endl;
}

int	Reader::_acceptConn(int listenFd) {
	struct sockaddr_storage	st;
	socklen_t				addrLen{sizeof st};
	char					s[INET_ADDRSTRLEN];
	int						fd;

	fd = accept(
		listenFd, reinterpret_cast<struct sockaddr*>(&st), &addrLen);
    if (fd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw wouldBlockException(); // No more incoming connections to accept
		throw std::runtime_error(std::string("FD ")
		+ std::to_string(listenFd) + ": [Reader] " + strerror(errno));
	}
	inet_ntop(st.ss_family, getInAddr(st), s, sizeof s);
	std::cout << "FD " << fd << ": [Reader] accepted connection from "
	<< s << ":" << ntohs(getPort(st)) << std::endl;
	return fd;

}

void	Reader::_receiveFromClient() {
     std::cout << BLUE << "FD " << _fd << ": [Reader] Reading from client.." 
     << RESET << std::endl;
    char buffer[BUFFER_SIZE];
        while (true) {
        ssize_t count = recv(_fd, buffer, sizeof(buffer), 0);
        if (0 < count)
        {
			size_t	prevLen = _request.size();
            _request.append(buffer, static_cast<size_t>(count));
            if ( _request.find("\r\n\r\n", prevLen) != std::string::npos ) {
                _complHeader = true;
				return;
			}
        }
        else if (-1 < count)
            throw std::runtime_error(
				std::string("FD ") + std::to_string(_fd) 
				+ ": [Reader] Client disconnected before completing header");
        else {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				throw wouldBlockException();
			if (errno == EINTR)
                continue;
            throw std::runtime_error(
				std::string("FD ") + std::to_string(_fd) 
				+ ": [Reader] Error receiving from client, " + strerror(errno));
        }
    }
}

const std::string&	Reader::getRequest() const {
	return _request;
}

bool	Reader::getcomplHeader() const {
	return _complHeader;
}

void    Reader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
		_printSocketError();
		std::cerr << "FD " << _fd
		<< ": [Reader] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		_receiveFromClient();
		new Writer(*this);
	}
	catch (const wouldBlockException& e) {
		return; // Nothing more to read now
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	delete this; // Will also remove from epoll
}