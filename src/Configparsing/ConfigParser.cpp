#include <algorithm>
#include "../../inc/Configparsing/ConfigParser.hpp"
#include "../../inc/Configparsing/WebservCoreModule.hpp"
#include "../../inc/Configparsing/WebservIndexModule.hpp"

ConfigParser::ConfigParser(char* filename): _configFilename(filename) {
	int ctxIndex = 0;
    _modules.push_back(std::make_unique<WebservCoreModule>(ctxIndex));
	_modules.push_back(std::make_unique<WebservIndexModule>(ctxIndex));
    _tokens = _tokenize();
}

ConfigParser::~ConfigParser() {
}

Tokens  ConfigParser::_tokenize() {
    std::ifstream   file(_configFilename, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0);

    std::string str(size, '\0');
    file.read(&str[0], size);

	Tokens tokens;
    std::size_t pos = 0;
    std::string word;
    while (pos != size) {
        if (isspace(str[pos])) {
            if (!word.empty()) tokens.push_back(word), word.clear();
            pos = str.find_first_not_of(" \t\r\n\f\v", pos);
        }
        else if (str[pos] == ';' || str[pos] == '{' || str[pos] == '}') {
            if (!word.empty()) tokens.push_back(word), word.clear();
            tokens.push_back(std::string(1, str[pos]));
            ++pos;
        }
        else {
            size_t  len = str.find_first_of(" \t\r\n\f\v;{}", pos) - pos;
            word = str.substr(pos, len);
            pos += len;
        }
    }
    if (!word.empty()) tokens.push_back(word);
	if (tokens.empty())
        throw std::runtime_error("Empty config file");
    return tokens;
}

void	ConfigParser::_validateBlockAllowedInLevel(
	const std::string& directive, WebservConfLevel level) {
	if ((_blockToValLevelMap.at(directive) & level)
	== static_cast<WebservConfLevel>(0))
		throw std::runtime_error("Unexpected '" + directive
			+ "' at level " + getLevelName(level));
}

void	ConfigParser::_parseBlock(
	const std::string& name, WebservConfLevel level) {
	_validateBlockAllowedInLevel(name, level);
	if (name == "http") {
		if (!_httpConfs.empty() || 0 < _servers.size())
			throw std::runtime_error(
				"Multiple 'http' blocks are not allowed");
		_confCtx.httpConfs = &_httpConfs;
	}
	else if (name == "server") {
		_servers.emplace_back(std::make_unique<IWebservModule::Srv>());
		mapAddrToServer(std::string(IP) + ":" + PORT, *_servers.back().get());
		_confCtx.srvConfs = &_servers.back().get()->srvConfs;
		_curLocNode = _servers.back().get()->location.get();
		_confCtx.locConfs = &_curLocNode->locConfs;
	}
	else if (name == "location") {
		_curLocNode->locations.emplace_back(
			std::make_unique<IWebservModule::LocNode>());
		_curLocNode->locations.back().get()->parent = _curLocNode;
		_curLocNode = _curLocNode->locations.back().get();
		_confCtx.locConfs = &_curLocNode->locConfs;
		if (!_tokens.empty() && _tokens.front() == "=")
			_curLocNode->matchType = 0, _tokens.pop_front();
		if (_tokens.empty() || _tokens.front() == "{"
			|| _tokens.front() == "}" || _tokens.front() == ";")
			throw std::runtime_error("Invalid Location");
		if (_tokens.front()[0] != '/')
			throw std::runtime_error(
				"Only exact and normal prefix matching implemented");
		_curLocNode->name = _tokens.front(), _tokens.pop_front();
	}
}

void    ConfigParser::_parseModuleDirective(WebservConfLevel level) {
	for (const auto& module: _modules) {
		if (module->isDirectiveValid(_tokens.front(), level)) {
            module->initConfIfEmptyAtLevel(getConfCtx(), level);
			module->parseDirective(*this, level);
			return;
		}
	}
	throw std::runtime_error("Unknown directive '"
		+ _tokens.front() + "' at level " + getLevelName(level));
}
void	ConfigParser::_validateLevel(WebservConfLevel level) {
	if (level != WebservConfLevel::MAIN
		&& level != WebservConfLevel::HTTP
		&& level != WebservConfLevel::SERVER
		&& level != WebservConfLevel::LOCATION)
		throw std::runtime_error("Invalid configuration level");
}

void	ConfigParser::parseConfig(WebservConfLevel level) {
	std::string	blockName;

	_validateLevel(level);
    while (!_tokens.empty()) {
		if (_tokens.front() == "}") {
			if (level == WebservConfLevel::MAIN)
				throw std::runtime_error(
					"Unexpected '}' at the end of MAIN block");
			if (level == WebservConfLevel::LOCATION)
				_curLocNode = _curLocNode->parent;
			_tokens.pop_front();
			return;
		}
        else if (_tokens.front() == "http"
		|| _tokens.front() == "server"
		|| _tokens.front() == "location") {
			blockName = _tokens.front(), _tokens.pop_front();
            _parseBlock(blockName, level);
            if (_tokens.empty() || _tokens.front() != "{")
                throw std::runtime_error("Expected '{' at the beginning of '" 
					+ blockName + "' block");
            _tokens.pop_front();
            parseConfig(level << 1);
        }
        else
			_parseModuleDirective(level);
    }
	if (level != WebservConfLevel::MAIN)
		throw std::runtime_error("Expected '}' at end of "
			+ getLevelName(level) + " block");
    mergeConfs();
}

void	ConfigParser::mergeConfs() {
	if (_httpConfs.empty())
		return;
	_confCtx.httpConfs = &_httpConfs;
	for (auto& srv : _servers) {
		_confCtx.srvConfs = &srv->srvConfs;
		_confCtx.locConfs = &srv->location->locConfs;
		for (const auto& module: _modules)
			module->mergeConfs(*this, srv->location);
	}
}

Tokens&	ConfigParser::getTokens() {
	return _tokens;
}

const IWebservModule::LocNode&	ConfigParser::getLocNode() const {
	return *_curLocNode;
}

const IWebservModule::ConfCtx&	ConfigParser::getConfCtx() const {
	return _confCtx;
}

const std::string&	ConfigParser::getLevelName(WebservConfLevel level) const {
	auto it = _levelNames.find(level);
	if (it != _levelNames.end())
		return it->second;
	else
		throw std::runtime_error("Unknown configuration level");
}

const IWebservModule::Srv&	ConfigParser::getLastSrv() const {
    if (_servers.empty())
        throw std::runtime_error(
            "Can't fetch the last server node as none available yet");
    return *_servers.back().get();
}

const ConfigParser::AddrToServersMap&	ConfigParser::getAddrToServersMap() const
{
	return _addrToServersMap;
}

void	ConfigParser::mapAddrToServer(const std::string& addr,
	const IWebservModule::Srv& srv) {
	if (!_addrToServersMap.count(addr)
		|| (std::find(_addrToServersMap[addr].begin(),
			_addrToServersMap[addr].end(),
			&srv) == _addrToServersMap[addr].end())
	) _addrToServersMap[addr].push_back(&srv);
}

void	ConfigParser::eraseMappingAddrToServer(const std::string& addr,
	const IWebservModule::Srv& srv) {
	if (_addrToServersMap.count(addr)) {
		auto it = std::find(_addrToServersMap[addr].begin(),
		_addrToServersMap[addr].end(), &srv);
		if (it != _addrToServersMap[addr].end())
			_addrToServersMap[addr].erase(it);
		if (_addrToServersMap[addr].empty())
			_addrToServersMap.erase(addr);
	}
}

void	ConfigParser::setTokens(const Tokens& tokens) {
	_tokens = tokens;
}