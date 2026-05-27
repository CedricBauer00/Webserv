#pragma once

#include <unistd.h>
// #include <string.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sys/epoll.h>

class AEventHandler;

class Epoller {
	private:
		const int	MAX_EVENTS{1024};
		const int	_epollfd;

	public:
		Epoller();
		~Epoller();
        int		getFd() const;
		void	addEventHandler(AEventHandler* handler, const uint32_t events) const;
        void    deleteEventHandler(AEventHandler* handler) const;
		void	runEventLoop() const;
};