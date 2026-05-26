#include <iostream>
#include "../inc/Listener.hpp"
#include "../inc/Epoller.hpp"

Listener::Listener(const std::string& addr, 
	const std::vector<const IWebservModule::SrvNode*>& servers,
	const Epoller* const epoller)
	: AEventHandler(_createListenSock(), servers, epoller), _addr(addr) {
    addSelfToEpoll(EPOLLIN | EPOLLET);
}

Listener::~Listener() {
}

int	Listener::_createListenSock() {
	int fd, rv, yes=1;
    struct addrinfo hints, *p;
	std::string	ip;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	size_t colonPos = _addr.find(':');
	ip = _addr.substr(0, colonPos);
	if (ip != "0.0.0.0") {
		rv = getaddrinfo(
			ip.c_str(), _addr.substr(colonPos + 1).c_str(), &hints, &p);
	}
	else {
		hints.ai_flags = AI_PASSIVE;
		rv = getaddrinfo(
			NULL, _addr.substr(colonPos + 1).c_str(), &hints, &p);
	}

	if (rv != 0) throw std::runtime_error(std::string("getaddrinfo: ")
		+ gai_strerror(rv));

	if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("socket: ") + strerror(errno));
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("setsockopt: ") + strerror(errno));
	}

	if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("bind: ") + strerror(errno));
	}
	freeaddrinfo(p);

	if (listen(fd, BACKLOG) == -1)
		throw std::runtime_error(std::string("listen: ") + strerror(errno));

	if (_setNonBlocking(fd) == -1)
		throw std::runtime_error(std::string("fcntl set: ") + strerror(errno));
	
	std::cout << "FD " << fd << ": Listening on " << _addr << std::endl;
	return fd;
}

void    Listener::_recreateSelf() {
    int err;
    socklen_t len = sizeof(err);

    getsockopt(_fd, SOL_SOCKET, SO_ERROR, &err, &len);
    std::cerr << "socket error: " << strerror(err) << std::endl;
    delSelfFromEpoll();
    closeFd();
    _fd = _createListenSock();
    addSelfToEpoll(EPOLLIN | EPOLLET);
}

void	Listener::process(uint32_t events) {
	struct sockaddr_storage	sockAddr;
	socklen_t				addrLen{sizeof sockAddr};
	char					s[INET_ADDRSTRLEN];
	int						clientFd;

    if (events & (EPOLLERR | EPOLLHUP)) {
        _recreateSelf();
        return;
    }
	while (true) {
		clientFd = accept(
			_fd, reinterpret_cast<struct sockaddr*>(&sockAddr), &addrLen);
		if (clientFd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break; // No more incoming connections to accept
			throw std::runtime_error(std::string("accept: ") + strerror(errno));
		}

		//printing
		inet_ntop(sockAddr.ss_family,
			getInAddr(reinterpret_cast<struct sockaddr*>(&sockAddr)),
			s,
			sizeof s);
		std::cout << "FD " << clientFd << ": accepted connection from " << s
		<< ":" << ntohs((reinterpret_cast<struct sockaddr*>(&sockAddr))->sin_port)
		<< std::endl;

		//Create Reader
		try {
			if (_setNonBlocking(clientFd) == -1)
				throw std::runtime_error(
					std::string("fcntl set: ") + strerror(errno));
			new Reader(clientFd, *this);
		}
		catch (const std::exception& e) {
			std::cerr << "Error creating Reader for fd " << clientFd
			<< ": " << e.what() << std::endl;
			if (close(clientFd) == -1)
				std::cerr << "Error closing fd "
				<< clientFd << ": " << strerror(errno) << std::endl;
		}
	}
}
