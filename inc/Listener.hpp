#pragma once

#include "AEventHandler.hpp"

class Listener: public AEventHandler
{
	private:
		const std::string&	_addr;

		int		_createListenFd(const std::string& addr);
        void	_recover();

	public:
		Listener() = delete;
		Listener(const std::string& addr,
			const std::vector<const IWebservModule::SrvNode*>& servers,
			const Epoller& epoller);
		virtual ~Listener();

		void	process(uint32_t events) override;
};
