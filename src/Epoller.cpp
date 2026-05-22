#include <unistd.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fcntl.h>
#include "../inc/Epoller.hpp"
#include "../inc/AEventHandler.hpp"

Epoller::Epoller() : _epollfd(epoll_create1(O_CLOEXEC)) {
	if (_epollfd == -1)
		throw std::runtime_error(std::string("epoll_create1: ") + strerror(errno));
	std::cout << "FD " << _epollfd << ": epoll instance created" << std::endl;
}

Epoller::~Epoller() {
	if (close(_epollfd) == -1)
		std::cerr << "Error closing epoll fd "
		<< _epollfd << ": " << strerror(errno) << std::endl;
	std::cout << "FD " << _epollfd << ": epoll closed" << std::endl;
}

void    Epoller::addEventHandler(AEventHandler* handler, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = handler;
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, handler->getFd(), &ev) == -1)
		throw std::runtime_error(std::string("epoll_ctl[add]: ") + strerror(errno));
	std::cout << "FD " << handler->getFd()
	<< ": added to epoll with events " << events << std::endl;
}

void	Epoller::runEventLoop() {
	struct epoll_event events[MAX_EVENTS];
	while (true) {
		int nfds = epoll_wait(_epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			if (errno == EINTR)
				continue; // Restart if interrupted by signal
			throw std::runtime_error(std::string("epoll_wait: ") + strerror(errno));
		}
		for (int n = 0; n < nfds; ++n) {
			try {
				static_cast<AEventHandler*>(events[n].data.ptr)->process();
			}
			catch (const std::exception& e) {
				std::cerr << "Error processing event for fd " 
				<< static_cast<AEventHandler*>(events[n].data.ptr)->getFd()
				<< ": " << e.what() << std::endl;
			}
		}
	}
}