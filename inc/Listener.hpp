#pragma once

#include "AEventHandler.hpp"

class Listener: public AEventHandler
{
	private:
		const unsigned int	BACKLOG{8192};
		std::string&		_addr;
	public:
		Listener(std::string& addr,
			std::vector<const IWebservModule::SrvNode*>& servers);
		virtual ~Listener();
		void	createListenSock();
};
