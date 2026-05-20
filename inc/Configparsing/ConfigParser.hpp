#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <unordered_map>
#include <memory>
#include "modules.hpp"
#include "IWebservModule.hpp"

class ConfigParser {
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
        char*						_configFilename;
        Tokens		                _tokens;
		VecOfPtrs<IWebservModule>	_modules;
		IWebservModule::ConfCtx		_confCtx;
		IWebservModule::LocNode*	_curLocNode{nullptr};

        Tokens  _tokenize();
		void    _parseModuleDirective(WebservConfLevel level);
		void    _parseBlock(const std::string& name, WebservConfLevel level);
		void	_validateLevel(WebservConfLevel level);
        void	_validateBlockAllowedInLevel(const std::string& directive,
            WebservConfLevel level);

    public:
		using AddrToServersMap =\
			std::unordered_map<std::string, std::vector<IWebservModule::SrvNode*>>;
		
		VecOfPtrs<IWebservModule::HttpConf>	httpConfs;
		VecOfPtrs<IWebservModule::SrvNode>	servers;
        AddrToServersMap					addrToServersMap;

        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void				parseConfig(WebservConfLevel level =
								WebservConfLevel::MAIN);
		Tokens&				getTokens();
		const IWebservModule::LocNode&	getLocNode();
		const IWebservModule::ConfCtx&	getConfCtx();
		const std::string&	getLevelName(WebservConfLevel level) const;

};
