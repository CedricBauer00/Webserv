#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include "configParsing.hpp"

class ConfigParser {
    private:
        char*						_configFilename;
        std::vector<std::string>	_tokens;
        WebservHttpConf*			_httpConf{nullptr};

        void	tokenize();
        size_t	parseHttpTopConfig(size_t i);
        size_t	parseHttpSrvConfig(size_t i);
		void	parseHttpSrvField(WebservSrvConf &srvConf, size_t i);
        size_t	parseHttpLocConfig(size_t i);
    public:
        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void	parseConfig();

};
