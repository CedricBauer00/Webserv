#include <string>
#include "../inc/AEventHandler.hpp"

AEventHandler::AEventHandler(
	std::vector<const IWebservModule::SrvNode*>& servers) : _servers(servers) {
}

AEventHandler::~AEventHandler() {
	if (_sockFd != -1) close(_sockFd);
}

const int	AEventHandler::getSockFd() const {
	return _sockFd;
}

int	AEventHandler::setNonBlocking() const {
    int flags = fcntl(_sockFd, F_GETFL, 0);

    if (flags < 0)
		throw std::runtime_error(std::string("fcntl read: ") + strerror(errno));

    return fcntl(_sockFd, F_SETFL, flags | O_NONBLOCK);
}
