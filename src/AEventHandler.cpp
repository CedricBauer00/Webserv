#include <string>
#include "../inc/AEventHandler.hpp"

AEventHandler::AEventHandler(WebservSocket&& sock,
	const uint32_t events,
	const Epoller& epoller,
	std::function<const Srv*(const std::string&)>&& selectServer)
: _sock(std::move(sock))
, _events(events)
, _epoller(epoller)
, _selectServer(std::move(selectServer)) {
	_epoller.addEventHandler(this, events);
}

AEventHandler::AEventHandler(AEventHandler&& other) noexcept
: _sock(std::move(other._sock))
, _events(other._events)
, _epoller(other._epoller)
, _selectServer(std::move(other._selectServer)) {
}

AEventHandler::~AEventHandler() {
    if (_sock.fd != -1) {
        _epoller.deleteEventHandler(this);
        closeFd();
    }
}

void	AEventHandler::_modifyEvent(const uint32_t events) {
	_epoller.modifyEventHandler(this, events);
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

void    AEventHandler::_printSocketError() {
    int err;
    socklen_t len = sizeof(err);

    getsockopt(_sock.fd, SOL_SOCKET, SO_ERROR, &err, &len);
    std::cerr << "FD " << _sock.fd << ": " << strerror(err) << std::endl;
}

std::function<const Srv*(const std::string&)>
AEventHandler::selectServerFactory(const std::vector<const Srv*>& srvs)
{
	if (srvs.empty())
		throw std::runtime_error("No servers configured");
	if (srvs.size() == 1)
		return [srv = srvs[0]](const std::string&) {
			return srv;
		};
	else
		return [&servers = srvs](const std::string& hostname) {
			const Srv* defaultSrv = nullptr;
			for (const Srv* srv : servers) {
				if (srv->srvConfs.empty())
					continue;
				const auto* srvCoreConf =\
				dynamic_cast<SrvCoreConf*>(srv->srvConfs[0].get());
				const auto& names = srvCoreConf->serverNames;
				if (names.find(hostname) != names.end())
					return srv;
				if (srvCoreConf->flags.has_value()
                && (srvCoreConf->flags.value() & DEFAULT_SERVER))
					defaultSrv = srv;
			}
			return defaultSrv != nullptr ? defaultSrv : servers[0];
		};
}

const std::vector<const Srv*>&	AEventHandler::getServers(
) const {
	return _servers;
}

const Epoller&	AEventHandler::getEpoller() const {
	return _epoller;
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

void	AEventHandler::closeFd() {
	if (close(_sock.fd) == -1)
		std::cerr << "FD " << _sock.fd << ": " << strerror(errno) << std::endl;
	std::cout << "FD " << _sock.fd << ": closed" << std::endl;
}

