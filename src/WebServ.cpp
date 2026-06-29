#include "../inc/WebServ.hpp"

WebServ::WebServ(char* configFilename)
	: _confParser(configFilename), _epoller() {
    _confParser.parseConfig();
	// std::cout << "WebServ created" << std::endl;
}

WebServ::~WebServ() {
	// std::cout << "WebServ destroyed" << std::endl;
}

void	WebServ::_createListeners() {
	if (_confParser.getAddrToServersMap().empty())
		throw std::runtime_error("No servers defined in config file");
	for (const auto& addrAndSrvNodes : _confParser.getAddrToServersMap()) {
		new Listener(addrAndSrvNodes.first,
            _epoller,
            AEventHandler::selectServerFactory(addrAndSrvNodes.second));
	}
}

void	WebServ::run() {
	_createListeners();
	_epoller.runEventLoop();
}