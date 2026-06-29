#include <string>
#include "../inc/AEventHandler.hpp"

AEventHandler::AEventHandler(WebservSocket&& sock,
	const uint32_t events,
	const Epoller& epoller,
    const std::function<const Srv*(const std::string&)>& selectServer)
: _sock(std::move(sock))
, _events(events)
, epoller(epoller)
, selectSrv(selectServer) {
    epoller.addEventHandler(this);
}

AEventHandler::AEventHandler(AEventHandler&& other) noexcept
: _sock(std::move(other._sock))
, _events(other._events)
, epoller(other.epoller)
, selectSrv(other.selectSrv) {
	epoller.modifyEventHandler(this);
}

AEventHandler::~AEventHandler() {
    if (_sock.fd != -1)
        epoller.deleteEventHandler(this);
}

int	AEventHandler::_dupFd(int fd) {
	int dupFd = dup(fd);

	if (dupFd == -1) {
		throw std::runtime_error(std::string("FD ")
		+ std::to_string(fd) + ": " + strerror(errno));
	}
	std::cout << "FD " << fd << ": fd duplicated" << std::endl;
	return dupFd;
}

void	AEventHandler::_setNonBlocking(int fd) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		throw std::runtime_error(std::string("fcntl read: ") + strerror(errno));
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl set: ") + strerror(errno));
}

void	AEventHandler::_modifyEvent(const uint32_t events) {
	_events = events;
}

void    AEventHandler::_printSocketError() {
    int err;
    socklen_t len = sizeof(err);

    getsockopt(_sock.fd, SOL_SOCKET, SO_ERROR, &err, &len);
    std::cerr << "FD " << _sock.fd << ": " << strerror(err) << std::endl;
}


uint32_t AEventHandler::getEvents() const {
	return _events;
}

int	AEventHandler::getFd() const {
	return _sock.fd;
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
