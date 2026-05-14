#include "../../inc/Configparsing/modules.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

AWebservParser::AWebservParser(
    int& ctxIndex,
	const std::unordered_map<std::string, WebservConfLevel> directiveValidLevels) :
	_ctxIndex(ctxIndex++),
    _directiveValidLevels(std::move(directiveValidLevels)) {
};

int AWebservParser::isDirectiveValid(
	const std::string& directive,
	WebservConfLevel level) {
	auto it = _directiveValidLevels.find(directive);
	return (it != _directiveValidLevels.end()
		&& (it->second & level) != static_cast<WebservConfLevel>(0));
};

void AWebservParser::_insertHttpConf(
    VecOfPtrs<WebservHttpConf>& httpConfs,
	std::unique_ptr<WebservHttpConf> conf) {
    if (httpConfs.size() <= static_cast<size_t>(_ctxIndex))
        httpConfs.resize(_ctxIndex + 1);
    httpConfs[_ctxIndex] = std::move(conf);
};

void AWebservParser::_insertSrvConf(
    VecOfPtrs<WebservSrvConf>& srvConfs, std::unique_ptr<WebservSrvConf> conf) {
    if (srvConfs.size() <= static_cast<size_t>(_ctxIndex))
        srvConfs.resize(_ctxIndex + 1);
    srvConfs[_ctxIndex] = std::move(conf);
};

void AWebservParser::_insertLocConf(
    VecOfPtrs<WebservLocConf>& locConfs, std::unique_ptr<WebservLocConf> conf) {
    if (locConfs.size() <= static_cast<size_t>(_ctxIndex))
        locConfs.resize(_ctxIndex + 1);
    locConfs[_ctxIndex] = std::move(conf);
};

WebservCoreParser::WebservCoreParser(int& ctxIndex) :
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
		{"location", WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
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
	static LocConf*	            curLocConf = nullptr;
	WebservSrvCoreConf* 		srvConf = nullptr;
    std::deque<std::string>&	tokens = parser.getTokens();
	std::string 				directive = tokens.front();
	tokens.pop_front();

	
	std::cout << directive << "---" << directiveMap.at(directive) << std::endl;
    switch (directiveMap.at(directive)) {
		case 0: // http
			if (tokens.empty() || tokens.front() != "{")
				throw std::runtime_error("Expected '{' after 'http'");
			tokens.pop_front();
			parser.httpConfCtx.httpConfs = &parser.httpConfs;
            _insertHttpConf(parser.httpConfs,
				std::make_unique<WebservHttpCoreConf>());
			// parser.httpConfs.resize(_ctxIndex + 1);
			// parser.httpConfs[_ctxIndex] = std::make_unique<WebservHttpCoreConf>();
			parser.parseConfig(WebservConfLevel::HTTP);
			break;
        case 1: // server
			if (tokens.empty() || tokens.front() != "{")
				throw std::runtime_error("Expected '{' after 'server'");
			tokens.pop_front();
			parser.servers.emplace_back();
            parser.httpConfCtx.srvConfs = &parser.servers.back().srvConfs;
            _insertSrvConf(parser.servers.back().srvConfs,
				std::make_unique<WebservSrvCoreConf>());
			// VecOfPtrs<WebservSrvConf>& srvConfs =\
			// 	parser.servers.back().srvConfs;
			// parser.httpConfCtx.srvConfs = &srvConfs;
			// srvConfs.resize(_ctxIndex + 1);
			// srvConfs[_ctxIndex] = std::make_unique<WebservSrvCoreConf>();
            curLocConf = &parser.servers.back().location;
			parser.httpConfCtx.locConfs = &curLocConf->locConfs;
            _insertLocConf(curLocConf->locConfs,
				std::make_unique<WebservLocCoreConf>());
			// curLocConf->locConfs.resize(_ctxIndex + 1);
			// curLocConf->locConfs[_ctxIndex] = std::make_unique<WebservLocCoreConf>();
			parser.parseConfig(WebservConfLevel::SERVER);
			break;
        case 2: // listen
			// parser.parseHttpSrvField(httpConf.servers.back(), 0);
			break;
        case 3: // server_name
			srvConf = dynamic_cast<WebservSrvCoreConf*>(
					parser.servers.back().srvConfs[_ctxIndex].get());
			while (!tokens.empty() && tokens.front() != ";"
				&& tokens.front() != "{" && tokens.front() != "}") {
				srvConf->serverNames.push_back(tokens.front());
				tokens.pop_front();
			}
        case 4: // num_req_expected
			(void)level;
			srvConf = dynamic_cast<WebservSrvCoreConf*>(
					parser.servers.back().srvConfs[_ctxIndex].get());
			srvConf->numReqExpected = std::stoi(tokens.front());
			if (srvConf->numReqExpected <= 0)
				throw std::runtime_error(
					"num_req_expected must be a non-zero positive integer");
			tokens.pop_front();
            break;
        case 5: // client_header_timeout
			(void)level;
			srvConf = dynamic_cast<WebservSrvCoreConf*>(
					parser.servers.back().srvConfs[_ctxIndex].get());
			srvConf->clientHeaderTimeout = WebservMsec(std::stoi(tokens.front()));
            break;
        case 6: // ignore_invalid_headers
            break;
        case 7: // merge_slashes
            break;
        case 8: // underscore_in_headers
            break;
		case 9: // location
			if (tokens.empty() || tokens.front() != "{")
				throw std::runtime_error("Expected '{' after 'location'");
			tokens.pop_front();
			curLocConf->locations.resize(curLocConf->locations.size() + 1);
			curLocConf->locations.back().parent = curLocConf;
			curLocConf = &curLocConf->locations.back();
			parser.httpConfCtx.locConfs = &curLocConf->locConfs;
            _insertLocConf(curLocConf->locConfs, std::make_unique<WebservLocCoreConf>());
			// curLocConf->locConfs.resize(_ctxIndex + 1);
			// curLocConf->locConfs[_ctxIndex] = std::make_unique<WebservLocCoreConf>();
			parser.parseConfig(WebservConfLevel::LOCATION);
			break;
        case 10: // root
            break;
        case 11: // allow
            break;
        case 12: // alias
            break;
        case 13: // client_body_buffer_size
            break;
        case 14: // client_body_timeout
            break;
        case 15: // client_max_body_size
            break;
        case 16: // send_timeout
            break;
        case 17: // absolute_redirect
            break;
        case 18: // log_not_found
            break;
        case 19: // error_page
            break;
        case 20: // index
            break;
        case 21: // autoindex
            break;
        case 22: // try_files
            break;
    }

	switch (directiveMap.at(directive)) {
		case 0: // empty the tokens thereby ignoring the rest after http block
			tokens.clear();
			break;
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
				throw std::runtime_error(
					"Expected ';' after directive '" + directive + "'");
			tokens.pop_front();
			break;
		default:
			throw std::runtime_error(
				"Parsing of directive: " + directive + " not implemented yet");
	}
}

WebservCoreModule::WebservCoreModule(int& ctxIndex) :
	WebservCoreParser(ctxIndex) {
}
