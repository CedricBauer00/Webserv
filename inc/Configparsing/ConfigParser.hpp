#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <unordered_map>
#include <memory>
#include "configParsing.hpp"
#include "modules.hpp"

class ConfigParser {
    private:
		const std::unordered_map<WebservConfLevel, std::string> _levelNames {
			{WebservConfLevel::MAIN, "main"},
			{WebservConfLevel::HTTP, "http"},
			{WebservConfLevel::SERVER, "server"},
			{WebservConfLevel::LOCATION, "location"},
		};
        char*							_configFilename;
        std::deque<std::string>			_tokens;
        WebservHttpConf					_httpConf;
		std::vector<IWebservModule*>	_modules;

        std::deque<std::string>	tokenize();
		void					parseDirective(WebservConfLevel level);
        size_t					parseHttpSrvConfig();
		void					parseHttpSrvField(WebservSrvConf &srvConf, size_t i);
        size_t					parseHttpLocConfig();

    public:
        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void						parseConfig(WebservConfLevel level = WebservConfLevel::MAIN);
		WebservHttpConf&			getHttpConf();
		std::deque<std::string>&	getTokens();
		const std::string&			getLevelName(WebservConfLevel level) const;

};
