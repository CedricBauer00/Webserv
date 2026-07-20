#pragma once

#include <signal.h>
#include "Configparsing/ConfigParser.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"
#include "Timer.hpp"

class WebServ {
	public:
		using AddrToSelectServerFcnMap =\
			std::unordered_map<std::string,
			std::function<const Srv*(const std::string&)>>;

	private:
		ConfigParser				_confParser;
		Epoller						_epoller;
        Timer						_timer;
		AddrToSelectServerFcnMap	_addrToSelectServerMap;

		std::function<const Srv*(const std::string&)>
		_selectServerFactory(const std::vector<const Srv*>& srvs);
		void	_makeAddrToSelectServerMap();
		void	_createListeners();
		void	_ignoreSIGPIPE();

	public:
		WebServ(char* configFilename);
		~WebServ();

		void	run();
};
