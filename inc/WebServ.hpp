#pragma once

#include "Configparsing/ConfigParser.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"

class WebServ {
	private:
		ConfigParser	_confParser;
		Epoller			_epoller;

		void	_createListeners();

	public:
		WebServ(char* configFilename);
		~WebServ();

		void	run();
};
