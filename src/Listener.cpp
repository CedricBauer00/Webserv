#include <iostream>
#include "../inc/Listener.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/Reader.hpp"

Listener::Listener(const std::string& addr, 
	const std::vector<const IWebservModule::SrvNode*>& servers,
	const Epoller& epoller)
	: AEventHandler(_createListenFd(addr), servers, epoller, EPOLLIN | EPOLLET),
	_addr(addr) {
}

Listener::~Listener() {
}

int	Listener::_createListenFd(const std::string& addr) {
    const unsigned int	BACKLOG{8192};
	int 				fd, rv, yes=1;
    struct addrinfo 	hints, *p;
	std::string			ip;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	size_t colonPos = addr.find(':');
	ip = addr.substr(0, colonPos);
	if (ip != "0.0.0.0") {
		rv = getaddrinfo(
			ip.c_str(), addr.substr(colonPos + 1).c_str(), &hints, &p);
	}
	else {
		hints.ai_flags = AI_PASSIVE;
		rv = getaddrinfo(
			NULL, addr.substr(colonPos + 1).c_str(), &hints, &p);
	}

	if (rv != 0) throw std::runtime_error(std::string("getaddrinfo: ")
		+ gai_strerror(rv));

	if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("socket: ") + strerror(errno));
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        close(fd);
		freeaddrinfo(p);
		throw std::runtime_error(std::string("setsockopt: ") + strerror(errno));
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
        close(fd);
		freeaddrinfo(p);
		throw std::runtime_error(std::string("setsockopt: ") + strerror(errno));
	}

	if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
        close(fd);
		freeaddrinfo(p);
		throw std::runtime_error(std::string("bind: ") + strerror(errno));
	}
	freeaddrinfo(p);

	if (listen(fd, BACKLOG) == -1) {
		close(fd);
		throw std::runtime_error(std::string("listen: ") + strerror(errno));
	}
	
	std::cout << "FD " << fd << ": [Listener] Listening on " << addr << std::endl;
	return fd;
}

void	Listener::_recover() {
	_printSocketError();

	try {
		new Listener(_addr, _servers, _epoller);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	delete this;
}

void	Listener::process(uint32_t events) {
	

    if (events & (EPOLLERR | EPOLLHUP))
		_recover();

	while (true) {
		try {
			new Reader(*this); //Create Reader
		}
		catch (const wouldBlockException& e) {
			break; // No more incoming connections to accept
		}
		catch (const std::exception& e) {
			std::cerr << "FD " << getFd() 
			<< ": [Listener] Error creating Reader, " << e.what() << std::endl;
		}
	}
}


