#include "../inc/Listener.hpp"

Listener::Listener(std::string& addr, 
	std::vector<const IWebservModule::SrvNode*>& servers)
	: AEventHandler(servers), _addr(addr) {
}

Listener::~Listener() {
}

void	Listener::createListenSock() {
	int rv, yes=1;
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

	if ((_sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("socket: ") + strerror(errno));
	}

	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("setsockopt: ") + strerror(errno));
	}

	if (bind(_sockFd, p->ai_addr, p->ai_addrlen) < 0) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("bind: ") + strerror(errno));
	}
	freeaddrinfo(p);

	if (listen(_sockFd, BACKLOG) < 0)
		throw std::runtime_error(std::string("listen: ") + strerror(errno));

	if (setNonBlocking() < 0)
		throw std::runtime_error(std::string("fcntl set: ") + strerror(errno));
	
	std::cout << "Listening on " << _addr << std::endl;	
}