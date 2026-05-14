#include "../../inc/Configparsing/ConfigParser.hpp"

ConfigParser::ConfigParser(char* filename): _configFilename(filename) {
	int ctxIndex = 0;
    _modules.push_back(std::make_unique<WebservCoreModule>(ctxIndex));
    _tokens = tokenize();
}

ConfigParser::~ConfigParser() {
}

std::deque<std::string>    ConfigParser::tokenize() {
    std::ifstream   file(_configFilename, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0);

    std::string str(size, '\0');
    file.read(&str[0], size);

	std::deque<std::string> tokens;
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
    int i = 0;
    for (auto& token: tokens) {
        std::cout << i << ": " << token << std::endl;
        ++i;
    }
    return tokens;
}

// size_t	ConfigParser::parseHttpLocConfig(size_t i) {
// 	if (_tokens.size() <= i || _tokens[i] != "{")
// 		throw std::runtime_error("Expected '{' after 'location'");
// 	_httpConf.servers.back().locations.resize(_httpConf->servers.back().locations.size() + 1);
// 	++i;
//     while (i < _tokens.size()) {
// 		if (_tokens[i] == "}")
// 			return;
//         ++i;
//     }
// 	throw std::runtime_error("Expected '}' at end of location block");
// }

// void	ConfigParser::parseHttpSrvField(WebservSrvConf &srvConf, size_t i) {
// 	std::unordered_map<std::string, int> elementMap = {
// 		{"listen", 0},
// 		{"server_name", 1},
// 		{"root", 2},
// 		{"num_req_expected", 3},
// 		{"client_header_timeout", 4},
// 		{"ignore_invalid_headers", 5},
// 		{"merge_slashes", 6},
// 		{"underscore_is_valid", 7}
// 	};

// 	std::string key = _tokens[i];
// 	++i;
// 	if (_tokens.size() <= i || _tokens[i] == ";"
// 		|| _tokens[i] == "{" || _tokens[i] == "}")
// 		throw std::runtime_error("Expected argument after '" + key + "'");
// 	switch (elementMap[key]) {
// 		case 0: // listen
// 			srvConf.flags |= LISTEN;
// 			break;
// 		case 1: // server_name
// 			while (1) {
// 				srvConf.serverNames.push(_tokens[i]);
// 				if (_tokens.size() <= i + 1 || _tokens[i + 1] == ";"
// 					|| _tokens[i + 1] == "{" || _tokens[i + 1] == "}")
// 					break;
// 				++i;
// 			}
// 			break;
// 		case 2: // root
// 			// srvConf.flags |= ROOT;
// 			break;
// 		case 3: // num_req_expected
// 			srvConf.numReqExpected = std::stoul(_tokens[i]);
// 			break;
// 		case 4: // client_header_timeout
// 			srvConf.clientHeaderTimeout = WebservMsec(std::stoul(_tokens[i]));
// 			break;
// 		case 5: // ignore_invalid_headers
// 			srvConf.ignore_invalid_headers = _tokens[i] == "true";
// 			break;
// 		case 6: // merge_slashes
// 			srvConf.merge_slashes = _tokens[i] == "true";
// 			break;
// 		case 7: // underscore_is_valid
// 			srvConf.underscore_is_valid = _tokens[i] == "true";
// 			break;
// 		default:
// 			throw std::runtime_error("Unknown server element: " + key);
// 	}
// 	if (_tokens.size() <= i + 1 || _tokens[i + 1] != ";")
// 		throw std::runtime_error("Expected ';' after the value of '" + key + "'");
// }

void    ConfigParser::parseDirective(WebservConfLevel level) {
	for (const auto& module: _modules) {
		if (module->isDirectiveValid(_tokens.front(), level)) {
			module->parseDirective(*this, level);
			return;
		}
	}
	throw std::runtime_error("Unknown directive '"
		+ _tokens.front() + "' at level " + getLevelName(level));
}

void	ConfigParser::parseConfig(WebservConfLevel level) {
    while (!_tokens.empty()) {
		if (_tokens.front() == "}") {
			if (level == WebservConfLevel::MAIN)
				throw std::runtime_error("Unexpected '}' at the end of MAIN block");
			return;
		}
		parseDirective(level);
    }
	if (level != WebservConfLevel::MAIN)
		throw std::runtime_error("Expected '}' at end of "
			+ getLevelName(level) + " block");
}

std::deque<std::string>&	ConfigParser::getTokens() {
	return _tokens;
}

const std::string&	ConfigParser::getLevelName(WebservConfLevel level) const {
	auto it = _levelNames.find(level);
	if (it != _levelNames.end())
		return it->second;
	else
		throw std::runtime_error("Unknown configuration level");
}
