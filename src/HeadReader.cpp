#include "../inc/HeadReader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Writer.hpp"

HeadReader::HeadReader(const Listener& listener)
    : AEventHandler(_acceptConn(listener.getFd()),
        listener.getServers(),
        listener.getEpoller(),
        EPOLLIN | EPOLLRDHUP | EPOLLET) {
}

HeadReader::~HeadReader() {
    std::cout << "FD " << _fd << ": [HeadReader] destroyed" << std::endl;
}

int	HeadReader::_acceptConn(int listenFd) {
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
		+ std::to_string(listenFd) + ": [HeadReader] " + strerror(errno));
	}
	inet_ntop(st.ss_family, getInAddr(st), s, sizeof s);
	std::cout << "FD " << fd << ": [HeadReader] accepted connection from "
	<< s << ":" << ntohs(getPort(st)) << std::endl;
	return fd;

}

void	HeadReader::_receiveFromClient() {
     std::cout << BLUE << "FD " << _fd << ": [HeadReader] Reading from client.." 
     << RESET << std::endl;
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t count = recv(_fd, buffer, sizeof(buffer), 0);
        if (0 < count)
        {
            std::string& request = _parser.getRequest();
            request.append(buffer, static_cast<std::size_t>(count));

            std::string::size_type pos = 0;
            while ((pos = request.find('\n')) != std::string::npos)
            {
                if (_parser.parseHead(pos))
                {
                    if (_parser.isComplHead())
                    return;
                    throw BadRequest();
                }
            }
            continue;
        }
        if (count == 0)
            throw std::runtime_error(
				std::string("FD ") + std::to_string(_fd)
				+ ": [HeadReader] Client disconnected before completing header");
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw wouldBlockException();
        if (errno == EINTR)
            continue;
        throw std::runtime_error(
				std::string("FD ") + std::to_string(_fd)
				+ ": [HeadReader] Error receiving from client, " + strerror(errno));
    }
}

void    HeadReader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
		_printSocketError();
		std::cerr << "FD " << _fd
		<< ": [HeadReader] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		_receiveFromClient();
		new Writer(*this, std::move(_parser), _selectServerFactory());
	}
	catch (const wouldBlockException& e) {
		return; // Nothing more to read now
	}
    catch (const HttpException& e) {
        std::cerr << "FD " << _fd << ": [HeadReader] HTTP error: " << std::endl;
    }
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	delete this; // Will also remove from epoll
}