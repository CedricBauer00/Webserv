#include <string>
#include "../inc/AEventHandler.hpp"
#include "../inc/Epoller.hpp"

AEventHandler::AEventHandler(
	const int fd,
	const std::vector<const IWebservModule::SrvNode*>& servers,
	const Epoller* const epoller)
    : _fd(fd), _servers(servers), _epoller(epoller) {
}

AEventHandler::~AEventHandler() {
    delSelfFromEpoll();
	closeFd();
}

int	AEventHandler::_setNonBlocking(int fd) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		throw std::runtime_error(std::string("fcntl read: ") + strerror(errno));
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

const std::vector<const IWebservModule::SrvNode*>&	AEventHandler::getServers(
) const {
	return _servers;
}

const Epoller* AEventHandler::getEpoller() const {
	return _epoller;
}

const int	AEventHandler::getFd() const {
	return _fd;
}

void	AEventHandler::closeFd() {
	if (close(_fd) == -1) {
		std::cerr << "Error closing fd "
		<< _fd << ": " << strerror(errno) << std::endl;
	}
	std::cout << "FD " << _fd << ": closed" << std::endl;
}

void	AEventHandler::addSelfToEpoll(uint32_t events) const {
	struct epoll_event ev;

	ev = {.events = events, .data = {.ptr = this}};
	if (epoll_ctl(_epoller->getFd(), EPOLL_CTL_ADD, _fd, &ev) == -1)
		throw std::runtime_error(
            std::string("epoll_ctl[add]: ") + strerror(errno));
	std::cout << "FD " << _fd
	<< ": added to epoll with events " << events << std::endl;
}

void	AEventHandler::delSelfFromEpoll() const {
	if (epoll_ctl(_epoller->getFd(), EPOLL_CTL_DEL, _fd, NULL) == -1)
        std::cerr << "Error deleting fd " << _fd
        << " from epoll: " << strerror(errno) << std::endl;
    else
        std::cout << "FD " << _fd << ": deleted from epoll" << std::endl;
}

void*	AEventHandler::getInAddr(struct sockaddr *sa) const {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
