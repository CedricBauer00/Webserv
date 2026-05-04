#include "../../inc/Configparsing/ConfigParser.hpp"

ConfigParser::ConfigParser(char* filename): _configFilename(filename) {
}

ConfigParser::~ConfigParser() {
	if (_httpConf)
		delete _httpConf;
}

void    ConfigParser::tokenize() {
    std::ifstream   file(_configFilename, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0);

    std::string str(size, '\0');
    file.read(&str[0], size);

    std::size_t pos = 0;
    std::string word;
    while (pos != size) {
        if (isspace(str[pos])) {
            if (!word.empty()) _tokens.push_back(word), word.clear();
            pos = str.find_first_not_of(" \t\r\n\f\v", pos);
        }
        else if (str[pos] == ';' || str[pos] == '{' || str[pos] == '}') {
            if (!word.empty()) _tokens.push_back(word), word.clear();
            _tokens.push_back(std::string(1, str[pos]));
            ++pos;
        }
        else {
            size_t  len = str.find_first_of(" \t\r\n\f\v;{}", pos) - pos;
            word = str.substr(pos, len);
            pos += len;
        }
    }
    if (!word.empty()) _tokens.push_back(word);
    for (auto token: _tokens)
        std::cout << token << std::endl;
}

void    ConfigParser::parseConfig() {
    tokenize();
    if (_tokens.empty())
        throw std::runtime_error("Empty config file");

    size_t  i = 0;
    while (i < _tokens.size()) {
        if (_tokens[i] == "http") {
            i = parseHttpTopConfig(++i);
        }
        i++;
    }
}
