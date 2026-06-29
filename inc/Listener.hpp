#pragma once

#include "AEventHandler.hpp"

class Listener: public AEventHandler
{
	private:
		const std::string	_addr;

        void	_recover();

	public:
		Listener() = delete;
		Listener(const std::string& addr,
			const Epoller& epoller,
			const std::function<const Srv*(const std::string&)>& selectServer);
		virtual ~Listener();

		static WebservSocket	createListenSock(const std::string& addr);

		void	process(uint32_t events) override;
};
