#pragma once

#include "AEventHandler.hpp"

class Listener: public AEventHandler
{
	private:
		const unsigned int	BACKLOG{8192};
		const std::string&		_addr;

		int	_createListenSock();

	public:
		Listener() = delete;
		Listener(const std::string& addr,
			const std::vector<const IWebservModule::SrvNode*>& servers,
			Epoller* const epoller);
		virtual ~Listener();

		void	process() override;
};
