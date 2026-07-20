#include <iostream>
#include "../inc/Timer.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/AEventHandler.hpp"

Timer::Timer(const Epoller& epoller)
: AEventHandler(_createTimer(),
	EPOLLIN | EPOLLET,
	epoller) {
	std::cout << "FD " << _sock.fd << ": [Timer] created" << std::endl;
}

Timer::~Timer() {
	std::cout << "FD " << _sock.fd << ": [Timer] destroyed" << std::endl;
}

WebservSocket	Timer::_createTimer() {
	int 			fd;
	itimerspec 		its;
	WebservSocket	ret;

	if ((fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK)) == -1)
		throw std::runtime_error(std::string("timer: ") + strerror(errno));

    its.it_value.tv_sec = IDLE_TIMEOUT;
	its.it_value.tv_nsec = 0;

    its.it_interval.tv_sec = IDLE_TIMEOUT;
	its.it_interval.tv_nsec = 0;

    if (timerfd_settime(fd, 0, &its, nullptr) == -1) {
		close(fd);
		throw std::runtime_error(std::string("timer: ") + strerror(errno));
	}

	ret.fd = fd;
	return ret;
}

void	Timer::setHandler(AEventHandler* handler) {
	handlers[handler->getFd()] = handler;
}

void	Timer::eraseHandler(const int fd) {
	if (handlers.find(fd) != handlers.end())
		handlers.erase(fd);
}

AEventHandler*	Timer::getHandler(const int fd) const {
	if (handlers.find(fd) != handlers.end())
		return handlers.at(fd);
	return nullptr;
}

void	Timer::process(uint32_t events) {
	(void)events;
	try {
		uint64_t expirations;
	
		ssize_t s = read(_sock.fd, &expirations, sizeof(expirations));
		if (s != sizeof(expirations)
		&& errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
			throw std::runtime_error(std::string("FD ") + std::to_string(_sock.fd)
			+ ": [Timer] read error: " + strerror(errno));
		}
		auto now = std::chrono::steady_clock::now();
		for (auto it = handlers.begin(); it != handlers.end(); ) {
			auto idle = std::chrono::duration_cast<std::chrono::seconds>(
				now - it->second->getLastActivity()).count();
			if (IDLE_TIMEOUT <= idle) {
				auto handler = it->second;
				++it;
				delete handler;
			}
			else
				++it;
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
