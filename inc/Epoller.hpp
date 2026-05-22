#pragma once

#include <sys/epoll.h>

class AEventHandler;

class Epoller {
	private:
		const int	MAX_EVENTS{1024};
		const int	_epollfd;

	public:
		Epoller();
		~Epoller();
		void	addEventHandler(AEventHandler* handler, uint32_t events);
		void	runEventLoop();
};