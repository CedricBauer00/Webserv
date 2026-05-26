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
	int fd, rv, yes=1;
    struct addrinfo hints, *p;
	std::string	ip;

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

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		freeaddrinfo(p);
		throw std::runtime_error(std::string("setsockopt: ") + strerror(errno));
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int)) == -1) {
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
	
	std::cout << "FD " << fd << ": Listening on " << addr << std::endl;
	return fd;
}

void	Listener::_recover(uint32_t events) {
	std::cerr << "FD " << getFd() 
	<< ": Got error event [type: " << events <<  "]" <<std::endl;

	try {
		new Listener(_addr, _servers, _epoller);
		delete this;
	}
	catch (const std::exception& e) {
		delete this;
		throw std::runtime_error(
			std::string("Error creating Listener: ") + e.what());
	}
}

void	Listener::_printAccept(struct sockaddr_storage& st) {
	char	s[INET_ADDRSTRLEN];

	inet_ntop(st.ss_family, getInAddr(st), s, sizeof s);
	std::cout << "FD " << _fd << ": accepted connection from " << s
	<< ":" << ntohs(getPort(st)) << std::endl;
}

void	Listener::process(uint32_t events) {
	struct sockaddr_storage	sockAddr;
	socklen_t				addrLen{sizeof sockAddr};
	int						clientFd;

    if (events & (EPOLLERR | EPOLLHUP))
        _recover(events);

	while (true) {
		clientFd = accept(
			_fd, reinterpret_cast<struct sockaddr*>(&sockAddr), &addrLen);
		if (clientFd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break; // No more incoming connections to accept
			throw std::runtime_error(std::string("accept: ") + strerror(errno));
		}
		_printAccept(sockAddr);		

		//Create Reader
		try {
			new Reader(clientFd, *this);
		}
		catch (const std::exception& e) {
			std::cerr << "FD " << getFd() << ": Error creating Reader for fd " 
			<< clientFd << ", " << e.what() << std::endl;
			closeFd(clientFd);
		}
	}
}


