#include "../../inc/Configparsing/modules.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

AWebservParser::AWebservParser(const std::unordered_map<std::string, WebservConfLevel> directiveValidLevels) : 
	_directiveValidLevels(std::move(directiveValidLevels)) {
};

int AWebservParser::isDirectiveValid(const std::string& directive, WebservConfLevel level) {
	auto it = _directiveValidLevels.find(directive);
	return it != _directiveValidLevels.end() && (it->second & level) != static_cast<WebservConfLevel>(0);
};

WebservCoreParser::WebservCoreParser() :
	AWebservParser({
            {"http", WebservConfLevel::MAIN},
			{"server", WebservConfLevel::HTTP},
			{"listen", WebservConfLevel::SERVER},
			{"server_name", WebservConfLevel::SERVER},
			{"num_req_expected", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"client_header_timeout", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"ignore_invalid_headers", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"merge_slashes", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"underscore_in_headers", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"root", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"allow", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"alias", WebservConfLevel::LOCATION},
			{"client_body_buffer_size", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"client_body_timeout", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"client_max_body_size", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"send_timeout", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"absolute_redirect", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"log_not_found", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"error_page", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"index", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"autoindex", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"try_files", WebservConfLevel::LOCATION},
		}) {
};

void WebservCoreParser::parseDirective(ConfigParser& parser, WebservConfLevel level) {
    static const std::unordered_map<std::string, int> directiveMap = [&]() {
        std::unordered_map<std::string, int> m;

        int i = 0;
        for (const auto& pair : _directiveValidLevels) {
            m[pair.first] = i++;
        }

        return m;
    }();
    std::deque<std::string>&	tokens = parser.getTokens();
    WebservHttpConf&			httpConf = parser.getHttpConf();
	const std::string& 			directive = tokens.front();
    switch (directiveMap.at(directive)) {
		case 0: // http
			if ((level & _directiveValidLevels.at(directive)) != static_cast<WebservConfLevel>(0)) {
				tokens.pop_front();
				if (tokens.empty() || tokens.front() != "{")
					throw std::runtime_error("Expected '{' after 'http'");
				tokens.pop_front();
				parser.parseConfig(WebservConfLevel::HTTP);
				break;
			}
			else
				throw std::runtime_error("'http' directive is not allowed in the " + parser.getLevelName(level) + " block");
        case 1: // server
			if ((level & _directiveValidLevels.at(directive)) != static_cast<WebservConfLevel>(0)) {
				tokens.pop_front();
				if (tokens.empty() || tokens.front() != "{")
					throw std::runtime_error("Expected '{' after 'server'");
				tokens.pop_front();
				httpConf.servers.emplace_back();
				parser.parseConfig(WebservConfLevel::SERVER);
				break;
			}
			else
				throw std::runtime_error("'server' directive is not allowed in the " + parser.getLevelName(level) + " block");
        case 2: // listen
            if ((level & _directiveValidLevels.at(directive)) != static_cast<WebservConfLevel>(0)) {
				parser.parseHttpSrvField(httpConf.servers.back(), 0);
				break;
			}
			else
				throw std::runtime_error("'listen' directive is not allowed in the " + parser.getLevelName(level) + " block");
        case 3: // server_name
            break;
        case 4: // num_req_expected
            break;
        case 5: // client_header_timeout
            break;
        case 6: // ignore_invalid_headers
            break;
        case 7: // merge_slashes
            break;
        case 8: // underscore_in_headers
            break;
        case 9: // root
            break;
        case 10: // allow
            break;
        case 11: // alias
            break;
        case 12: // client_body_buffer_size
            break;
        case 13: // client_body_timeout
            break;
        case 14: // client_max_body_size
            break;
        case 15: // send_timeout
            break;
        case 16: // absolute_redirect
            break;
        case 17: // log_not_found
            break;
        case 18: // error_page
            break;
        case 19: // index
            break;
        case 20: // autoindex
            break;
        case 21: // try_files
            break;
        default:
            throw std::runtime_error("Parsing of directive: " + tokens.front() + " not implemented yet");
    }
}
