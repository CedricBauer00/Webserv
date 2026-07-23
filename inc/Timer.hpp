#pragma once

#include <sys/timerfd.h>
#include "../inc/AEventHandler.hpp"

constexpr int IDLE_TIMEOUT = 600; // seconds

class Timer: public AEventHandler
{
	private:
		std::unordered_map<int, AEventHandler*>	handlers;

		WebservSocket	_createTimer();

	public:
		Timer() = delete;
		Timer(const Epoller& epoller);
		virtual ~Timer();

		void			setHandler(AEventHandler* handler);
		void			eraseHandler(const int fd);
		AEventHandler*	getHandler(const int fd) const;
		void			process(uint32_t events) override;
};