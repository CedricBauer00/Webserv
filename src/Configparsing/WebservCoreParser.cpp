#include <array>
#include "../../inc/Configparsing/WebservCoreParser.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

WebservCoreParser::WebservCoreParser(int& ctxIndex) :
	AWebservParser(
        ctxIndex,
        {{"listen", WebservConfLevel::SERVER},
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
        std::array<std::string, 20> directives = {
            "listen", "server_name", "num_req_expected",
            "client_header_timeout", "ignore_invalid_headers", "merge_slashes",
            "underscore_in_headers", "root", "allow", "alias",
            "client_body_buffer_size", "client_body_timeout",
            "client_max_body_size",
            "send_timeout", "absolute_redirect", "log_not_found",
            "error_page", "index", "autoindex", "try_files"
        };
        int i = 0;
        for (const auto& directive : directives) {
            std::cout << directive << i << std::endl;
            m[directive] = i++;
        }
        return m;
    }();
	SrvCoreConf*				srvConf = nullptr;
    std::deque<std::string>&	tokens = parser.getTokens();
	std::string					directive = tokens.front();
	tokens.pop_front();

	switch (level) {
	case WebservConfLevel::HTTP:
		_ensureConfExists(parser.confCtx.httpConfs,
						  []()
						  { return std::make_unique<HttpCoreConf>(); });
		break;
	case WebservConfLevel::SERVER:
		_ensureConfExists(parser.confCtx.srvConfs,
						  []()
						  { return std::make_unique<SrvCoreConf>(); });
		break;
	case WebservConfLevel::LOCATION:
		_ensureConfExists(parser.confCtx.locConfs,
						  []()
						  { return std::make_unique<LocCoreConf>(); });
		break;
	default:
		throw std::runtime_error("Invalid configuration level");
		break;
	};
	std::cout << directive << "---" << directiveMap.at(directive) << std::endl;
    switch (directiveMap.at(directive)) {
        case 0: // listen
			// parser.parseHttpSrvField(httpConf.servers.back(), 0);
			break;
        case 1: // server_name
			srvConf = dynamic_cast<SrvCoreConf*>(
					(*parser.confCtx.srvConfs)[_ctxIndex].get());
			while (!tokens.empty() && tokens.front() != ";"
				&& tokens.front() != "{" && tokens.front() != "}") {
				srvConf->serverNames.push_back(tokens.front());
				tokens.pop_front();
			}
            break;
        case 2: // num_req_expected
			(void)level;
			srvConf = dynamic_cast<SrvCoreConf*>(
					(*parser.confCtx.srvConfs)[_ctxIndex].get());
			srvConf->numReqExpected = std::stoi(tokens.front());
			if (srvConf->numReqExpected <= 0)
				throw std::runtime_error(
					"num_req_expected must be a non-zero positive integer");
			tokens.pop_front();
            break;
        case 3: // client_header_timeout
			(void)level;
			srvConf = dynamic_cast<SrvCoreConf*>(
					(*parser.confCtx.srvConfs)[_ctxIndex].get());
			srvConf->clientHeaderTimeout = WebservMsec(std::stoi(tokens.front()));
			tokens.pop_front();
            break;
        case 4: // ignore_invalid_headers
            break;
        case 5: // merge_slashes
            break;
        case 6: // underscore_in_headers
            break;
        case 7: // root
            break;
        case 8: // allow
            break;
        case 9: // alias
            break;
        case 10: // client_body_buffer_size
            break;
        case 11: // client_body_timeout
            break;
        case 12: // client_max_body_size
            break;
        case 13: // send_timeout
            break;
        case 14: // absolute_redirect
            break;
        case 15: // log_not_found
            break;
        case 16: // error_page
            break;
        case 17: // index
            break;
        case 18: // autoindex
            break;
        case 19: // try_files
            break;
    }
	if (tokens.empty() || tokens.front() != ";")
	throw std::runtime_error("Expected ';' after directive '" + directive + "'");
	tokens.pop_front();
};


