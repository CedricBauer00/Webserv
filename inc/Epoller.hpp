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
        const int	getFd() const;
		void	addEventHandler(AEventHandler* handler, uint32_t events);
        void    deleteEventHandler(AEventHandler* handler);
		void	runEventLoop();
};