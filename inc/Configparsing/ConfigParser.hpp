#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <unordered_map>
#include <memory>
#include "modules.hpp"

class ConfigParser {
	private:
		const std::unordered_map<WebservConfLevel, std::string> _levelNames {
			{WebservConfLevel::MAIN, "main"},
			{WebservConfLevel::HTTP, "http"},
			{WebservConfLevel::SERVER, "server"},
			{WebservConfLevel::LOCATION, "location"},
		};
        const std::unordered_map<std::string, WebservConfLevel> _directiveValLevelMap {
            {"http", WebservConfLevel::MAIN},
            {"server", WebservConfLevel::HTTP},
            {"location", WebservConfLevel::SERVER | WebservConfLevel::LOCATION}
        };
        char*						_configFilename;
        std::deque<std::string>		_tokens;
		VecOfPtrs<IWebservModule>	_modules;

        std::deque<std::string>	_tokenize();
		void					_parseModuleDirective(WebservConfLevel level);
        int                     _isDirectiveNotInValidLevel(
                                    const std::string& directive,
                                    WebservConfLevel level);

    public:
		WebservHttpConfCtx				httpConfCtx;
		VecOfPtrs<WebservHttpConf>		httpConfs;
		std::vector<SrvLocConf>			servers;

        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void						parseConfig(WebservConfLevel level =
										WebservConfLevel::MAIN);
		std::deque<std::string>&	getTokens();
		const std::string&			getLevelName(WebservConfLevel level) const;

};
