#include <array>
#include <algorithm>
#include <cctype>
#include "../../inc/Configparsing/WebservCoreParser.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"
#include "../../inc/MethodTypes.hpp"

WebservCoreParser::WebservCoreParser(int& ctxIndex) :
	AWebservParser(ctxIndex) {
};

const std::unordered_map<
std::string,
std::pair<WebservConfLevel, AWebservParser::parseFunc>>&	WebservCoreParser::_getParseMap() {
	return _parseMap;
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
		srvConf->flags.value() |= DEFAULT_SERVER, t.pop_front();
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
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
        _addLowerLevelDirective(d,
            {t.front()},
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
    else if (!dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->alias.has_value()
	&& dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->root.empty())
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->root = t.front();
	t.pop_front();
}

void WebservCoreParser::_parseAllow(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    unsigned int mask = 0;
    Tokens values;

	while (1) {
        if (methodMap.count(t.front()))
            if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
                values.push_back(t.front());
            else
                mask |= methodMap.at(t.front()).second;
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

void WebservCoreParser::_parseAlias(Tokens& t, const ConfCtx& c) {
	if (!dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->alias.has_value()) {
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->alias = true;
		dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->root = t.front();
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

