#pragma once

#include "Configparsing/ConfigParser.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"

constexpr const char*	RED = "\033[31m";
constexpr const char*	ELEC_RED = "\033[38;2;255;20;20m";
constexpr const char*	BLUE = "\033[34m";
constexpr const char*	GREEN = "\033[32m";
constexpr const char*	ORANGE = "\033[38;2;255;120;0m";
constexpr const char*	RESET = "\033[0m";

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
