#include <array>
#include <map>
#include <algorithm>
#include <cctype>
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
    const ConfCtx&	confCtx = parser.getConfCtx();
    Tokens&			tokens = parser.getTokens();
	std::string		directive = tokens.front();
	tokens.pop_front();

	if (tokens.empty())
		throw std::runtime_error(
			"Incomplete Config: Nothing after '" + directive + "'");
    _initConfIfEmptyAtLevel(confCtx, level);
    switch (directiveMap.at(directive)) {
        case 0: // listen
			_parseListen(tokens, confCtx, parser);
			break;
        case 1: // server_name
			_parseServerNames(directive, tokens, confCtx);
            break;
        case 2: // num_req_expected
			_parseNumReqExpected(directive, tokens, confCtx, level);
            break;
        case 3: // client_header_timeout
			_parseClientHeaderTimeout(directive, tokens, confCtx, level);
            break;
        case 4: // ignore_invalid_headers
        case 5: // merge_slashes
        case 6: // underscore_in_headers
			_parseBoolDirective(directive, tokens, confCtx, level);;
			break;
        case 7: // root
			_parseRoot(directive, tokens, confCtx, level);
			break;
        case 8: // allow
			_parseAllow(directive, tokens, confCtx, level);
			break;
        case 9: // alias
			_parseAlias(directive, tokens, confCtx, level, parser);
			break;
        case 10: // client_body_buffer_size
			_parseClientBodyBufferSize(directive, tokens, confCtx, level);
			break;
        case 11: // client_body_timeout
			_parseClientBodyTimeout(directive, tokens, confCtx, level);
			break;
        case 12: // client_max_body_size
			_parseClientMaxBodySize(directive, tokens, confCtx, level);
			break;
        case 13: // send_timeout
			_parseSendTimeout(directive, tokens, confCtx, level);
			break;
        case 14: // absolute_redirect
        case 15: // log_not_found
			_parseBoolDirective(directive, tokens, confCtx, level);
			break;
        case 16: // error_page
			_parseErrorPage(directive, tokens, confCtx, level);
			break;
        case 17: // index
			_parseIndex(directive, tokens, confCtx, level);
			break;
        case 18: // autoindex
			_parseAutoindex(directive, tokens, confCtx, level);
			break;
        case 19: // try_files
			_parseTryFiles(directive, tokens, confCtx, level);
			break;
    }
	if (tokens.empty() || tokens.front() != ";")
		throw std::runtime_error(
			"Expected ';' after directive '" + directive + "'");
	tokens.pop_front();
};

void	WebservCoreParser::_initConfIfEmptyAtLevel(
	const ConfCtx& confCtx, WebservConfLevel level) {
	switch (level) {
		case WebservConfLevel::HTTP:
			_ensureConfExists(confCtx.httpConfs,
							[]()
							{ return std::make_unique<HttpCoreConf>(); });
			break;
		case WebservConfLevel::SERVER:
			_ensureConfExists(confCtx.srvConfs,
							[]()
							{ return std::make_unique<SrvCoreConf>(); });
			break;
		case WebservConfLevel::LOCATION:
			_ensureConfExists(confCtx.locConfs,
							[]()
							{ return std::make_unique<LocCoreConf>(); });
			break;
		default:
			throw std::runtime_error("Invalid configuration level");
			break;
	};
};

bool	WebservCoreParser::_isValueValid(const std::string& tok) {
	return (tok != ";" && tok != "{" && tok != "}");
};
void	WebservCoreParser::_validateValue(const std::string& directive,
	const std::string& val) {
	if (!_isValueValid(val))
		throw std::runtime_error("Invalid value '" + val
            + "'for directive '" + directive + "'");
}

bool	WebservCoreParser::_parseBooleanValue(const std::string& directive,
    const std::string& tok) {
    if (tok == "on" || tok == "true" || tok == "1")
        return true;
    else if (tok == "off" || tok == "false" || tok == "0")
        return false;
    else
        throw std::runtime_error("Invalid boolean value '" + tok
            + "'for directive '" + directive + "'");
};

void	WebservCoreParser::_addLowerLevelDirective(
	const std::string& directive,
    std::vector<std::string> vals,
    std::vector<std::vector<std::string>>& arr) {
    vals.insert(vals.begin(), directive);
    arr.push_back(vals);
}

void	WebservCoreParser::_parseListen(Tokens& t, const ConfCtx& c,
	ConfigParser& parser) {
	std::string	ip;
	std::string	port;
	std::string	str = t.front();
	t.pop_front();

	size_t colonPos = str.find(':');
	if (colonPos != std::string::npos) {
		ip = str.substr(0, colonPos);
		port = str.substr(colonPos + 1);
	}
	else {
		if (str.find('.') != std::string::npos)
			ip = str, port = "80";
		else
			ip = "0.0.0.0", port = str;
	}
	if (!isValidIpv4(ip))
		throw std::runtime_error("Invalid listen IP '" + ip + "'");
	if (!isValidPort(port))
		throw std::runtime_error("Invalid listen port '" + port + "'");

	std::string	addr = ip + ":" + port;
	SrvNode* node = parser.servers.back().get();
	ConfigParser::AddrToServersMap& map = parser.addrToServersMap;
	map[addr].push_back(node);
	std::cout << map.at(addr).back() << std::endl;
	SrvCoreConf* srvConf = dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c));
	srvConf->flags = LISTEN;
	if (!t.empty() && t.front() == "default_server")
		srvConf->flags |= DEFAULT_SERVER, t.pop_front();
	
};

void	WebservCoreParser::_parseServerNames(const std::string& directive,
	Tokens& t, const ConfCtx& c) {
	SrvCoreConf* srvConf = dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c));
	_validateValue(directive, t.front());
	while (!t.empty() && _isValueValid(t.front())) {
		srvConf->serverNames.push_back(t.front());
		t.pop_front();
	}
};

void WebservCoreParser::_parseNumReqExpected(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	_validateValue(directive, t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->numReqExpected =\
        std::stoul(t.front());
	t.pop_front();
}

void WebservCoreParser::_parseClientHeaderTimeout(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	_validateValue(directive, t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->clientHeaderTimeout =\
        WebservMsec(std::stoul(t.front()));
	t.pop_front();
}

void WebservCoreParser::_parseBoolDirective(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    bool b = _parseBooleanValue(d, t.front());
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else {
        if (d == "ignore_invalid_headers"
            || d == "merge_slashes"
            || d == "underscore_in_headers") {
            SrvCoreConf* srvConf = dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c));
            if (d == "ignore_invalid_headers") srvConf->ignore_invalid_headers = b;
            else if (d == "merge_slashes") srvConf->merge_slashes = b;
			else srvConf->underscore_is_valid = b;
        }
        else {
            LocCoreConf* locConf = dynamic_cast<LocCoreConf*>(getLocConfPtr(c));
            if (d == "absolute_redirect") locConf->absoluteRedirect = b;
			else locConf->logNotFound = b;
        }
    }
	t.pop_front();
}

void WebservCoreParser::_parseRoot(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	_validateValue(directive, t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else {
		LocCoreConf* locConf = dynamic_cast<LocCoreConf*>(getLocConfPtr(c));
		locConf->root = t.front();
	}
	t.pop_front();
}

void WebservCoreParser::_parseAllow(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    static const std::unordered_map<std::string, unsigned int> m = {
        {"GET", 1u<<0}, {"POST", 1u<<1}, {"PUT", 1u<<2},
        {"DELETE", 1u<<3}, {"HEAD", 1u<<4}, {"OPTIONS", 1u<<5}};
    unsigned int mask = 0;
    std::vector<std::string> values;
    if (!m.count(t.front())) throw std::runtime_error("Invalid value '" + t.front() 
    + "'for directive '" + directive + "'");
	while (!t.empty() && _isValueValid(t.front())) {
        if (m.count(t.front()))
            if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
                values.push_back(t.front());
            else
                mask |= m.at(t.front());
        else
            throw std::runtime_error("Invalid value '" + t.front()
                + "'for directive '" + directive + "'");
		t.pop_front();
	}
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            values,
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		(void)mask;
}

void WebservCoreParser::_parseAlias(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l, ConfigParser& parser) {
	const LocNode&	curLocNode = parser.getLocNode();
	LocCoreConf* locConf = dynamic_cast<LocCoreConf*>(getLocConfPtr(c));
	locConf->alias = curLocNode.name.size();
	_parseRoot(directive, t, c, l);
}

void WebservCoreParser::_parseClientBodyBufferSize(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    _validateValue(directive, t.front());
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->clientBodyBufferSize =\
        std::stoul(t.front());
	t.pop_front();
}

void WebservCoreParser::_parseClientBodyTimeout(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	_validateValue(directive, t.front());
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->clientBodyTimeout =\
        WebservMsec(std::stoul(t.front()));
	t.pop_front();
}

void WebservCoreParser::_parseClientMaxBodySize(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    _validateValue(directive, t.front());
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->clientMaxBodySize =\
        std::stoul(t.front());
	t.pop_front();
}

void WebservCoreParser::_parseSendTimeout(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    _validateValue(directive, t.front());
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->sendTimeout =\
        WebservMsec(std::stoul(t.front()));
	t.pop_front();
}

void WebservCoreParser::_parseErrorPage(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	(void)directive;
    (void)t;
    (void)c;
    (void)l;
}

void WebservCoreParser::_parseIndex(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    std::vector<std::string> values;
    _validateValue(directive, t.front());
	while (!t.empty() && _isValueValid(t.front())) {
		values.push_back(t.front());
		t.pop_front();
	}
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            values,
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		(void)values;
}

void WebservCoreParser::_parseAutoindex(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    bool b = _parseBooleanValue(directive, t.front());
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		(void)b;
    t.pop_front();
}

void WebservCoreParser::_parseTryFiles(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	(void)directive;
    (void)t;
    (void)c;
    (void)l;
}

