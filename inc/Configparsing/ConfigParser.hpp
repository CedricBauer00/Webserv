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
        char*											_configFilename;
        std::deque<std::string>							_tokens;
		std::vector<std::unique_ptr<IWebservModule>>	_modules;
		std::vector<std::unique_ptr<WebservHttpConf>>	_httpConfs;
        struct  SrvLocConfs {
            std::vector<std::unique_ptr<WebservSrvConf>>	srvConfs;
            std::vector<std::unique_ptr<WebservLocConf>>	LocConfs;
        };
        std::vector<SrvLocConfs>						_servers;

        std::deque<std::string>	tokenize();
		void					parseDirective(WebservConfLevel level);

    public:
		struct	WebservHttpConfCtx {
			std::vector<std::unique_ptr<WebservHttpConf>>* httpConfs;
			std::vector<std::unique_ptr<WebservSrvConf>>* srvConfs;
			std::vector<std::unique_ptr<WebservLocConf>>* locConfs;
		};

        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void						parseConfig(WebservConfLevel level = WebservConfLevel::MAIN);
		WebservHttpConf&			getHttpConf();
		std::deque<std::string>&	getTokens();
		const std::string&			getLevelName(WebservConfLevel level) const;

};
