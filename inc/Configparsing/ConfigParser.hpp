#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <unordered_map>
#include <memory>
#include "modules.hpp"
#include "IWebservModule.hpp"

class ConfigParser {
    public:
		using AddrToServersMap =\
			std::unordered_map<std::string,
			std::vector<const IWebservModule::SrvNode*>>;

        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void							parseConfig(WebservConfLevel level
			= WebservConfLevel::MAIN);
		Tokens&							getTokens();
		const IWebservModule::LocNode&	getLocNode() const;
		const IWebservModule::ConfCtx&	getConfCtx() const;
		const std::string&				getLevelName(WebservConfLevel level) const;
		const IWebservModule::SrvNode*	getLastSrvNode() const;
		const AddrToServersMap&			getAddrToServersMap() const;
		void							mapAddrToServer(const std::string& addr,
			const IWebservModule::SrvNode*);
		void							eraseMappingAddrToServer(
			const std::string& addr, const IWebservModule::SrvNode* node);

	private:
		const std::unordered_map<WebservConfLevel, std::string> _levelNames {
			{WebservConfLevel::MAIN, "main"},
			{WebservConfLevel::HTTP, "http"},
			{WebservConfLevel::SERVER, "server"},
			{WebservConfLevel::LOCATION, "location"},
		};
        const std::unordered_map<std::string, WebservConfLevel>
			_blockToValLevelMap {
				{"http", WebservConfLevel::MAIN},
				{"server", WebservConfLevel::HTTP},
				{"location", WebservConfLevel::SERVER
					| WebservConfLevel::LOCATION}
        };
        char*								_configFilename;
        Tokens		                		_tokens;
		VecOfPtrs<IWebservModule>			_modules;
		IWebservModule::ConfCtx				_confCtx;
		IWebservModule::LocNode*			_curLocNode{nullptr};
        VecOfPtrs<IWebservModule::HttpConf>	_httpConfs;
		VecOfPtrs<IWebservModule::SrvNode>	_servers;
        AddrToServersMap					_addrToServersMap;

        Tokens  _tokenize();
		void    _parseModuleDirective(WebservConfLevel level);
		void    _parseBlock(const std::string& name, WebservConfLevel level);
		void	_validateLevel(WebservConfLevel level);
        void	_validateBlockAllowedInLevel(const std::string& directive,
            WebservConfLevel level);

};
