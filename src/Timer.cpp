#include "../inc/Timer.hpp"

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
    its.it_interval.tv_sec = IDLE_TIMEOUT;

    if (timerfd_settime(timerfd, 0, &its, nullptr) == -1) {
		close(fd);
		throw std::runtime_error(std::string("timer: ") + strerror(errno));
	}

	ret.fd = fd;
	memset(ret.ss.get(), 0, sizeof *ret.ss);
	return ret;
}

void	Timer::addHandler(std::unique_ptr<AEventHandler>&& handler) {
	handlers[handler->getFd()] = std::move(handler);
	// the handler will be automatically destroyed when it is removed from the map
}

void	Timer::process(uint32_t events) {
	try {
		uint64_t expirations;
	
		ssize_t s = read(_sock.fd, &expirations, sizeof(expirations));
		if (s != sizeof(expirations)
		&& errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
			throw std::runtime_error(std::string("FD ") + std::to_string(_sock.fd)
			+ ": [Timer] read error: " + strerror(errno));
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		exit;
	}
}
