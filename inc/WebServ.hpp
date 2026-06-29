#pragma once

#include "Configparsing/ConfigParser.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"

class WebServ {
	public:
		using AddrToSelectServerFcnMap =\
			std::unordered_map<std::string,
			std::function<const Srv*(const std::string&)>>;

	private:
		ConfigParser				_confParser;
		Epoller						_epoller;
		AddrToSelectServerFcnMap	_addrToSelectServerMap;

		std::function<const Srv*(const std::string&)>
		_selectServerFactory(const std::vector<const Srv*>& srvs);
		void	_makeAddrToSelectServerMap();
		void	_createListeners();

	public:
		WebServ(char* configFilename);
		~WebServ();

		void	run();
};
