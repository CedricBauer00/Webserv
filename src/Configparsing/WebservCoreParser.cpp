#include <array>
#include <algorithm>
#include <cctype>
#include "../../inc/Configparsing/WebservCoreParser.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

WebservCoreParser::WebservCoreParser(int& ctxIndex) :
	AWebservParser(ctxIndex) {
};

const std::unordered_map<
std::string,
std::pair<WebservConfLevel, AWebservParser::parseFunc>>&	WebservCoreParser::_getParseMap() {
	return _parseMap;
};

void WebservCoreParser::parseDirective(
	ConfigParser& parser,
	WebservConfLevel level) {
	// static const std::unordered_map<
	// std::string,
	// std::function<void(const std::string& d, Tokens& t,
	// 	const ConfCtx& c, WebservConfLevel l, ConfigParser& p)>
	// 	> parseMap = {
	// 		{"listen", [this](const std::string& d, Tokens& t, const ConfCtx& c,
	// 			WebservConfLevel l, ConfigParser& p) {
	// 			(void)d; (void)l;
	// 			_parseListen(t, c, p);
	// 		}},
	// 		{"server_name", [this](const std::string& d, Tokens& t, const ConfCtx& c,
	// 			WebservConfLevel l, ConfigParser& p) {
	// 			(void)d; (void)l; (void)p;
	// 			_parseServerNames(t, c);
	// 		}},
	// 		{"num_req_expected", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseNumReqExpected(d, t, c, l);
	// 		}},
	// 		{"client_header_timeout", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseClientHeaderTimeout(d, t, c, l);
	// 		}},
	// 		{"ignore_invalid_headers", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseIgnoreInvalidHeaders(d, t, c, l);
	// 		}},
	// 		{"merge_slashes", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseMergeSlashes(d, t, c, l);
	// 		}},
	// 		{"underscore_in_headers", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseUnderscoreInHeaders(d, t, c, l);
	// 		}},
	// 		{"root", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseRoot(d, t, c, l);
	// 		}},
	// 		{"allow", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseAllow(d, t, c, l);
	// 		}},
	// 		{"alias", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			_parseAlias(d, t, c, l, p);
	// 		}},
	// 		{"client_body_buffer_size", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseClientBodyBufferSize(d, t, c, l);
	// 		}},
	// 		{"client_body_timeout", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseClientBodyTimeout(d, t, c, l);
	// 		}},
	// 		{"client_max_body_size", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseClientMaxBodySize(d, t, c, l);
	// 		}},
	// 		{"send_timeout", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseSendTimeout(d, t, c, l);
	// 		}},
	// 		{"absolute_redirect", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseAbsoluteRedirect(d, t, c, l);
	// 		}},
	// 		{"log_not_found", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseLogNotFound(d, t, c, l);
	// 		}},
	// 		{"error_page", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseErrorPage(d, t, c, l);
	// 		}},
	// 		{"try_files", [this](const std::string& d, Tokens& t,
	// 			const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
	// 			(void)p;
	// 			_parseTryFiles(d, t, c, l);
	// 		}},
	// };
    const ConfCtx&	confCtx = parser.getConfCtx();
    Tokens&			tokens = parser.getTokens();
	std::string		directive = tokens.front();

	tokens.pop_front();
	if (tokens.empty() || _isDelimiter(tokens.front()))
		throw std::runtime_error(
			"Invalid definition for directive '" + directive + "'");
    _initConfIfEmptyAtLevel<HttpCoreConf, SrvCoreConf, LocCoreConf>(
		confCtx, level);
	_parseMap.at(directive).second(directive, tokens, confCtx, level, parser);
	if (tokens.empty() || tokens.front() != ";")
		throw std::runtime_error(
			"Invalid definition for directive '" + directive + "'");
	tokens.pop_front();
};

void	WebservCoreParser::_parseListen(Tokens& t, const ConfCtx& c,
	ConfigParser& p) {
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
			ip = str, port = PORT;
		else
			ip = IP, port = str;
	}
	if (!isValidIpv4(ip))
		throw std::runtime_error("Invalid listen IP '" + ip + "'");
	if (!isValidPort(port))
		throw std::runtime_error("Invalid listen port '" + port + "'");

    if (ip != IP || port != PORT) {
        p.mapAddrToServer(ip + ":" + port, p.getLastSrv());
        p.eraseMappingAddrToServer(std::string(IP) + ":" + PORT,
            p.getLastSrv());
    }
	SrvCoreConf* srvConf = dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c));
	srvConf->flags = LISTEN;
	if (!t.empty() && t.front() == "default_server")
		srvConf->flags |= DEFAULT_SERVER, t.pop_front();
};

void	WebservCoreParser::_parseServerNames(Tokens& t, const ConfCtx& c) {
	SrvCoreConf* srvConf = dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c));
	while (1) {
		srvConf->serverNames.insert(t.front());
		t.pop_front();
        if (t.empty() || _isDelimiter(t.front()))
            break;
	}
};

void WebservCoreParser::_parseNumReqExpected(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->numReqExpected,
			std::stoul(t.front()));
	t.pop_front();
}

void WebservCoreParser::_parseClientHeaderTimeout(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->clientHeaderTimeout,
			WebservMsec(std::stoul(t.front())));
	t.pop_front();
}

void WebservCoreParser::_parseIgnoreInvalidHeaders(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	bool b = _parseBooleanValue(t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->ignore_invalid_headers,
			b);
	t.pop_front();
}

void WebservCoreParser::_parseMergeSlashes(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	bool b = _parseBooleanValue(t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->merge_slashes,
			b);
	t.pop_front();
}

void WebservCoreParser::_parseUnderscoreInHeaders(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	bool b = _parseBooleanValue(t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c))->underscore_is_valid,
			b);
	t.pop_front();
}

void WebservCoreParser::_parseRoot(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	if (!dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->alias.has_value()) {
		if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
			_addLowerLevelDirective(d,
				{t.front()},
				dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
		else
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->root = t.front();
	}
	t.pop_front();
}

void WebservCoreParser::_parseAllow(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    static const std::unordered_map<std::string, unsigned int> m = {
        {"GET", 1u<<0}, {"POST", 1u<<1}, {"PUT", 1u<<2},
        {"DELETE", 1u<<3}, {"HEAD", 1u<<4}, {"OPTIONS", 1u<<5}};
    unsigned int mask = 0;
    std::vector<std::string> values;

	while (1) {
        if (m.count(t.front()))
            if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
                values.push_back(t.front());
            else
                mask |= m.at(t.front());
        else
            throw std::runtime_error("Invalid value '" + t.front()
                + "'for directive '" + d + "'");
		t.pop_front();
        if (t.empty() || _isDelimiter(t.front()))
            break;
	}
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            values,
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->allowedMethods,
			mask);
}

void WebservCoreParser::_parseAlias(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l, ConfigParser& parser) {
	if (!dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->alias.has_value()) {
		const LocNode&	curLocNode = parser.getLocNode();
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->alias = curLocNode.name.size();
		_parseRoot(d, t, c, l);
	}
	else {
		t.pop_front();
	}
}

void WebservCoreParser::_parseClientBodyBufferSize(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->clientBodyBufferSize =\
        std::stoul(t.front());
	t.pop_front();
}

void WebservCoreParser::_parseClientBodyTimeout(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->clientBodyTimeout,
        	WebservMsec(std::stoul(t.front())));
	t.pop_front();
}

void WebservCoreParser::_parseClientMaxBodySize(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->clientMaxBodySize,
			std::stoul(t.front()));
	t.pop_front();
}

void WebservCoreParser::_parseSendTimeout(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->sendTimeout,
			WebservMsec(std::stoul(t.front())));
	t.pop_front();
}

void WebservCoreParser::_parseAbsoluteRedirect(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	bool b = _parseBooleanValue(t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->absoluteRedirect,
			b);
	t.pop_front();
}

void WebservCoreParser::_parseLogNotFound(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	bool b = _parseBooleanValue(t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->logNotFound,
			b);
	t.pop_front();
}

void WebservCoreParser::_parseErrorPage(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	(void)d;
    (void)t;
    (void)c;
    (void)l;
}

void WebservCoreParser::_parseTryFiles(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	(void)d;
    (void)t;
    (void)c;
    (void)l;
}

