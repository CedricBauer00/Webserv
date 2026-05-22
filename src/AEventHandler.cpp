#include <string>
#include "../inc/AEventHandler.hpp"
#include "../inc/Epoller.hpp"

AEventHandler::AEventHandler(
	const int fd,
	const std::vector<const IWebservModule::SrvNode*>& servers,
	Epoller* const epoller)
    : _fd(fd), _servers(servers), _epoller(epoller) {
}

AEventHandler::~AEventHandler() {
	if (close(_fd) == -1) {
		std::cerr << "Error closing fd "
		<< _fd << ": " << strerror(errno) << std::endl;
	}
	std::cout << "FD " << _fd << ": closed" << std::endl;
}

int	AEventHandler::_setNonBlocking(int fd) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		throw std::runtime_error(std::string("fcntl read: ") + strerror(errno));

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

const std::vector<const IWebservModule::SrvNode*>&	AEventHandler::getServers() const
{
	return _servers;
}

Epoller* const AEventHandler::getEpoller() const {
	return _epoller;
}

const int	AEventHandler::getFd() const {
	return _fd;
}

void*	AEventHandler::getInAddr(struct sockaddr *sa) const {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
