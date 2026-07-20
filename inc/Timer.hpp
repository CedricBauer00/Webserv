#pragma once

#include <sys/timerfd.h>
#include "AEventHandler.hpp"

constexpr int IDLE_TIMEOUT = 30; // seconds

class Timer: public AEventHandler
{
	private:
		std::unordered_map<int, std::unique_ptr<AEventHandler>>	handlers;

		WebservSocket	_createTimer();

	public:
		Timer() = delete;
		Timer(const Epoller& epoller);
		virtual ~Timer();

		void	addHandler(std::unique_ptr<AEventHandler>&& handler);
		void	process(uint32_t events) override;
}