#pragma once

#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <memory>
#include "configParsing.hpp"
#include "Module.hpp"

class ConfigParser {
    private:
        char*							_configFilename;
        std::queue<std::string>			_tokens;
        std::vector<WebservHttpConf>	_httpConf;
		std::vector<IWebservModule*>	_modules;

        void	tokenize();
        size_t	parseHttpTopConfig();
        size_t	parseHttpSrvConfig();
		void	parseHttpSrvField(WebservSrvConf &srvConf, size_t i);
        size_t	parseHttpLocConfig();
    public:
        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void	parseConfig();

};
