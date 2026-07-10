#include "../inc/Epoller.hpp"
#include "../inc/AEventHandler.hpp"

Epoller::Epoller() : _epollfd(epoll_create1(O_CLOEXEC)) {
	if (_epollfd == -1)
		throw std::runtime_error(
            std::string("epoll_create1: ") + strerror(errno));
	std::cout << "FD " << _epollfd << ": epoll instance created" << std::endl;
}

Epoller::~Epoller() {
	if (close(_epollfd) == -1)
		std::cerr << "FD " << _epollfd << ": " << strerror(errno) << std::endl;
	std::cout << "FD " << _epollfd << ": epoll closed" << std::endl;
}

const char*	Epoller::_eventsToStr(uint32_t events) const {
	for (size_t i = 0; i < evToStr.size(); ++i) {
		if (events == evToStr[i].first)
			return evToStr[i].second;
	}
	return "UNKNOWN";
}

int	Epoller::getFd() const {
	return _epollfd;
}

void    Epoller::addEventHandler(AEventHandler* handler) const {
	struct epoll_event	ev;
    int					fd = handler->getFd();

	ev.events = handler->getEvents();
	ev.data.ptr = handler;
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw std::runtime_error(std::string("FD ")
			+ std::to_string(fd)
            + ": <epoll_ctl[add]> " + strerror(errno));
	// std::cout << "FD " << fd
	// << ": added to epoll with events " << _eventsToStr(ev.events) << std::endl;
}

void    Epoller::modifyEventHandler(AEventHandler* handler) const {
	struct epoll_event	ev;
	int 				fd = handler->getFd();

	ev.events = handler->getEvents();
	ev.data.ptr = handler;
	if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev) == -1)
		throw std::runtime_error(std::string("FD ")
			+ std::to_string(fd)
			+ ": <epoll_ctl[mod]> " + strerror(errno));
	// std::cout << "FD " << fd
	// << ": modified in epoll with events " << _eventsToStr(ev.events) << std::endl;
}

void    Epoller::deleteEventHandler(AEventHandler* handler) const {
    int fd = handler->getFd();

    if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        std::cerr << "FD " << fd
		<< ": <epoll_ctl[del]> " << strerror(errno) << std::endl;
        return;
    }
    // std::cout << "FD " << fd
    // << ": deleted from epoll" << std::endl;
}

void	Epoller::runEventLoop() const {
	struct epoll_event events[MAX_EVENTS];

	while (true) {
		int nfds = epoll_wait(_epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			if (errno == EINTR)
				continue; // Restart if interrupted by signal
			throw std::runtime_error(
                std::string("FATAL ERROR [epoll_wait]: ") + strerror(errno));
			// heap memory is not freed here. Doing so requires storing
			// allocations as a list and free'ing them here, which by design 
			// is avoided here to speed up event processing.
		}
		for (int n = 0; n < nfds; ++n) {
			static_cast<AEventHandler*>(events[n].data.ptr)->process(
					events[n].events);
		}
	}
}
