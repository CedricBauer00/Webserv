#include <array>
#include <algorithm>
#include <cctype>
#include <sstream>
#include "../../inc/Configparsing/WebservCoreParser.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"
#include "../../inc/MethodTypes.hpp"
#include "../../inc/statusCodes.hpp"

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

	SrvCoreConf* srvConf = dynamic_cast<SrvCoreConf*>(getSrvConfPtr(c));
	srvConf->flags = LISTEN;
	if (!t.empty() && t.front() == "default_server")
		srvConf->flags.value() |= DEFAULT_SERVER, t.pop_front();

	if (ip != IP || port != PORT) {
        p.mapAddrToServer(ip + ":" + port, p.getLastSrv(), srvConf->flags.value());
        p.eraseMappingAddrToServer(std::string(IP) + ":" + PORT,
            p.getLastSrv());
    }
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
        auto it = methodMap.find(t.front());
        if (it != methodMap.end())
            if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
                values.push_back(t.front());
            else
                mask |= it->second.first;
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
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
        _addLowerLevelDirective(d,
            _getDirectiveVals(t),
            dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
    else {
		auto assertErrCode = [](unsigned long n, unsigned long excp) {
			if (0 < excp && n == excp)
				return;
			if (statusCodeToReasonPhrase.find(n) == statusCodeToReasonPhrase.end()
			|| n < 300) {
				std::vector<int> keys;
				for (const auto& item : statusCodeToReasonPhrase)
					keys.push_back(item.first);
				std::sort(keys.begin(), keys.end());
				std::ostringstream	msg;
				msg << "Wrong error code! Use only these: ";
				for (auto key : keys)
					if (300 <= key) msg << key << " ";
				throw std::runtime_error(msg.str());
			}
		};
		std::unordered_set<unsigned long>	errCodes;
		unsigned long						num;
		unsigned long						resCode{0};

		while (!t.empty() && isDigits(t.front())) {
			num = std::stoul(t.front());
			assertErrCode(num, 0);
			errCodes.insert(num);
			t.pop_front();
		}
		if (errCodes.empty() || t.empty())
			throw std::runtime_error("Invalid 'error_page' directive");
		if (t.front()[0] == '=') {
			if (!isDigits(t.front().substr(1)))
				throw std::runtime_error("Invalid 'error_page' directive");
			resCode = std::stoul(t.front().substr(1));
			assertErrCode(resCode, 200);
			t.pop_front();
			if (t.empty())
				throw std::runtime_error("Invalid 'error_page' directive");
		}
		if (resCode != 0)
			for (auto code: errCodes)
				dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->errPages[code] =\
				{resCode, t.front()};
		else
			for (auto code: errCodes)
				dynamic_cast<LocCoreConf*>(getLocConfPtr(c))->errPages[code] =\
				{code, t.front()};
		t.pop_front();
	}
}

void WebservCoreParser::_parseTryFiles(const std::string& d,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	(void)d;
    (void)t;
    (void)c;
    (void)l;
}
