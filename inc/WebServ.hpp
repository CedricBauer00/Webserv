#pragma once

#include "Configparsing/ConfigParser.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"

class WebServ {
	private:
		ConfigParser			_confParser;
		Epoller					_epoller;
		std::vector<Listener>	_listeners;

		void	_createListeners();

	public:
		WebServ(char* configFilename);
		~WebServ();
		void	run();
};
