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

void WebservIndexParser::parseDirective(
	ConfigParser& parser,
	WebservConfLevel level) {
	const ConfCtx&	confCtx = parser.getConfCtx();
	Tokens&			tokens = parser.getTokens();
	std::string		directive = tokens.front();

	tokens.pop_front();
	if (tokens.empty() || _isDelimiter(tokens.front()))
		throw std::runtime_error(
			"Invalid definition for directive '" + directive + "'");
	_initConfIfEmptyAtLevel<HttpIndexConf, SrvIndexConf, LocIndexConf>(
		confCtx, level);
	_parseMap.at(directive).second(directive, tokens, confCtx, level, parser);
	if (tokens.empty() || tokens.front() != ";")
		throw std::runtime_error(
			"Invalid definition for directive '" + directive + "'");
	tokens.pop_front();
};

void WebservIndexParser::_parseIndex(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
    std::vector<std::string> values;
	while (1) {
		values.push_back(t.front());
		t.pop_front();
        if (t.empty() || _isDelimiter(t.front()))
            break;
	}
    if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
		_addLowerLevelDirective(directive,
            values,
            dynamic_cast<HttpIndexConf*>(
				getHttpConfPtr(c))->lowerLevelDirectives);
	else
		dynamic_cast<LocIndexConf*>(getLocConfPtr(c))->indexFiles =\
		std::move(values);
}

void WebservIndexParser::_parseAutoindex(const std::string& directive,
	Tokens& t, const ConfCtx& c, WebservConfLevel l) {
	try {
		bool b = _parseBooleanValue(t.front());
		if ((l & WebservConfLevel::HTTP) != static_cast<WebservConfLevel>(0))
			_addLowerLevelDirective(directive,
				{t.front()},
				dynamic_cast<HttpIndexConf*>(
					getHttpConfPtr(c))->lowerLevelDirectives);
		else
			dynamic_cast<LocIndexConf*>(
				getLocConfPtr(c))->autoindex = b;
		t.pop_front();
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Invalid value '" + t.front()
			+ "' for directive '" + directive + "': " + e.what());
	}
}