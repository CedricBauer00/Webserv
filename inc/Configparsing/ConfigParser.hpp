#pragma once

#include <iostream>
#include <fstream>
#include "ConfigParsing.hpp"

class ConfigParser {
    private:
        char*   _configFilename;
        std::vector<std::string> _tokens;
        Global _global;

        void tokenize();

    public:
        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void parseConfig();
        Global getGlobal();

};
