#include "../../inc/Configparsing/modules.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

AWebservParser::AWebservParser(
    const int ctxIndex,
	const std::unordered_map<std::string, WebservConfLevel> directiveValidLevels) : 
	_ctxIndex(ctxIndex),
    _directiveValidLevels(std::move(directiveValidLevels)) {
};

int AWebservParser::isDirectiveValid(
	const std::string& directive,
	WebservConfLevel level) {
	auto it = _directiveValidLevels.find(directive);
	return (it != _directiveValidLevels.end()
		&& (it->second & level) != static_cast<WebservConfLevel>(0));
};

WebservCoreParser::WebservCoreParser(const int ctxIndex) :
	AWebservParser(
        ctxIndex,
        {{"http", WebservConfLevel::MAIN},
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

void WebservCoreParser::parseDirective(
	ConfigParser& parser,
	WebservConfLevel level) {
    static const std::unordered_map<std::string, int> directiveMap = [&]() {
        std::unordered_map<std::string, int> m;

        int i = 0;
        for (const auto& pair : _directiveValidLevels) {
            m[pair.first] = i++;
        }

        return m;
    }();
    std::deque<std::string>&	tokens = parser.getTokens();
	std::string 				directive = tokens.front();
    WebservHttpConf&			httpConf = parser.getHttpConf();
	WebservSrvConf*				srvConf = nullptr;
	
	if (!httpConf.servers.empty())
		srvConf = &httpConf.servers.back();
	tokens.pop_front();

    switch (directiveMap.at(directive)) {
		case 0: // http
			if (tokens.empty() || tokens.front() != "{")
				throw std::runtime_error("Expected '{' after 'http'");
			tokens.pop_front();
			parser.parseConfig(WebservConfLevel::HTTP);
			break;
        case 1: // server
			if (tokens.empty() || tokens.front() != "{")
				throw std::runtime_error("Expected '{' after 'server'");
			tokens.pop_front();
			httpConf.servers.emplace_back();
			// add server pointer to server context
			parser.parseConfig(WebservConfLevel::SERVER);
			break;
        case 2: // listen
			parser.parseHttpSrvField(httpConf.servers.back(), 0);
			break;
        case 3: // server_name
			while (!tokens.empty() && tokens.front() != ";"
				&& tokens.front() != "{" && tokens.front() != "}") {
				srvConf->serverNames.push_back(tokens.front());
				tokens.pop_front();
			}
        case 4: // num_req_expected
			srvConf->numReqExpected = std::stoi(tokens.front());
			if (srvConf->numReqExpected <= 0)
				throw std::runtime_error(
					"num_req_expected must be a non-zero positive integer");
			tokens.pop_front();
            break;
        case 5: // client_header_timeout
			srvConf->clientHeaderTimeout = std::stoi(tokens.front());
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
    }

	switch (directiveMap.at(directive)) {
		case 0:
		case 1:
            tokens.pop_front(); // pop '{'
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
			if (tokens.empty() || tokens.front() != ";")
				throw std::runtime_error("Expected ';' after directive '" + directive + "'");
			tokens.pop_front();
			break;
		default:
			throw std::runtime_error("Parsing of directive: " + directive + " not implemented yet");
	}
}

WebservCoreModule::WebservCoreModule(const int ctxIndex) : WebservCoreParser(ctxIndex) {
}
