#pragma once

#include "AEventHandler.hpp"

class Listener: public AEventHandler
{
	private:
		const unsigned int	BACKLOG{8192};
		const std::string&		_addr;

		int		_createListenSock();
        void	_recreateSelf();

	public:
		Listener() = delete;
		Listener(const std::string& addr,
			const std::vector<const IWebservModule::SrvNode*>& servers,
			const Epoller* const epoller);
		virtual ~Listener();

		void	process(uint32_t events) override;
};
