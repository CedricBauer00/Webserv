#include "../inc/WebServ.hpp"

WebServ::WebServ(char* configFilename)
: _confParser(configFilename)
, _epoller()
, _addrToSelectServerMap() {
    _confParser.parseConfig();
	_makeAddrToSelectServerMap();
	// std::cout << "WebServ created" << std::endl;
}

WebServ::~WebServ() {
	// std::cout << "WebServ destroyed" << std::endl;
}

std::function<const Srv*(const std::string&)>
WebServ::_selectServerFactory(const std::vector<const Srv*>& srvs)
{
	if (srvs.empty())
		throw std::runtime_error("No servers configured");
	if (srvs.size() == 1)
		return [srv = srvs[0]](const std::string&) {
			return srv;
		};
	else
		return [&servers = srvs](const std::string& hostname) {
			for (const Srv* srv : servers) {
				if (srv->srvConfs.empty() || !srv->srvConfs[0])
					continue;
				const auto& names =\
				dynamic_cast<SrvCoreConf*>(srv->srvConfs[0].get())->serverNames;
				if (names.find(hostname) != names.end())
					return srv;
			}
			return servers[0];
		};
}

void	WebServ::_makeAddrToSelectServerMap() {
	if (_confParser.getAddrToServersMap().empty())
		throw std::runtime_error("No servers defined in config file");
	for (const auto& addrAndSrvNode : _confParser.getAddrToServersMap()) {
		_addrToSelectServerMap[addrAndSrvNode.first] =\
			_selectServerFactory(addrAndSrvNode.second);
	}
	
}

void	WebServ::_createListeners() {
	for (const auto& addrAndSelectServerNode : _addrToSelectServerMap) {
		new Listener(addrAndSelectServerNode.first,
            _epoller,
            addrAndSelectServerNode.second);
	}
}

void	WebServ::run() {
	_createListeners();
	_epoller.runEventLoop();
}