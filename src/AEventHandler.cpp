#include <string>
#include "../inc/AEventHandler.hpp"
#include "../inc/Epoller.hpp"

AEventHandler::AEventHandler(
	const int fd,
	const std::vector<const IWebservModule::Srv*>& servers,
	const Epoller& epoller,
	const uint32_t events)
    : _fd(fd), _servers(servers), _epoller(epoller) {
    try {
        _setNonBlocking(_fd);
        _epoller.addEventHandler(this, events);
    }
    catch (const std::exception& e) {
        throw;
    }
}

AEventHandler::~AEventHandler() {
	_epoller.deleteEventHandler(this);
	closeFd(_fd);
}

void	AEventHandler::_setNonBlocking(int fd) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		throw std::runtime_error(std::string("fcntl read: ") + strerror(errno));
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl set: ") + strerror(errno));
}

void    AEventHandler::_printSocketError() {
    int err;
    socklen_t len = sizeof(err);

    getsockopt(_fd, SOL_SOCKET, SO_ERROR, &err, &len);
    std::cerr << "FD " << _fd << ": " << strerror(err) << std::endl;
}

const std::vector<const IWebservModule::Srv*>&	AEventHandler::getServers(
) const {
	return _servers;
}

const Epoller&	AEventHandler::getEpoller() const {
	return _epoller;
}

int	AEventHandler::getFd() const {
	return _fd;
}

void	AEventHandler::closeFd(int fd) {
	if (close(fd) == -1)
		std::cerr << "FD " << fd << ": " << strerror(errno) << std::endl;
	std::cout << "FD " << fd << ": closed" << std::endl;
}

void*	AEventHandler::getInAddr(struct sockaddr_storage& st) const {
    if (st.ss_family == AF_INET) {
        return &(((struct sockaddr_in&)st).sin_addr);
    }
    return &(((struct sockaddr_in6&)st).sin6_addr);
}

in_port_t	AEventHandler::getPort(struct sockaddr_storage& st) const {
    if (st.ss_family == AF_INET) {
        return ((struct sockaddr_in&)st).sin_port;
    }
    return ((struct sockaddr_in6&)st).sin6_port;
}