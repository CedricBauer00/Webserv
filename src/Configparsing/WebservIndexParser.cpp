#include "../../inc/Configparsing/WebservIndexParser.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

WebservIndexParser::WebservIndexParser(int& ctxIndex) :
	AWebservParser(
		ctxIndex) {
};

const std::unordered_map<
std::string,
std::pair<WebservConfLevel, AWebservParser::parseFunc>>&	WebservIndexParser::_getParseMap() {
	return _parseMap;
};

void WebservIndexParser::_parseIndex(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    Tokens values;
	while (1) {
		values.push_back(t.front());
		t.pop_front();
        if (t.empty() || _isDelimiter(t.front()))
            break;
	}
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            values,
            dynamic_cast<HttpIndexConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocIndexConf*>(getLocConfPtr(c))->indexFiles =\
		std::move(values);
}

void WebservIndexParser::_parseAutoindex(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	bool b = _parseBooleanValue(t.front());
	if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
			{t.front()},
			dynamic_cast<HttpIndexConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
	else
		_assignIfHasNoValue(
			dynamic_cast<LocIndexConf*>(getLocConfPtr(c))->autoindex, b);
	t.pop_front();
}