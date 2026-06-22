#include "../../inc/Configparsing/AWebservParser.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"

AWebservParser::AWebservParser(
    int& ctxIndex) :
	_ctxIndex(ctxIndex++) {
};

bool AWebservParser::isDirectiveValid(
	const std::string& directive,
	WebservConfLevel level) {
	auto&	parseMap =  _getParseMap();
	auto it = parseMap.find(directive);
	return (it != parseMap.end()
		&& (it->second.first & level) != static_cast<WebservConfLevel>(0));
};

WebservConfLevel	AWebservParser::getLowestValidLevelOfDirective(
	const std::string& directive) {
	WebservConfLevel	lowVallevel = WebservConfLevel::HTTP;
	if (_getParseMap().find(directive) == _getParseMap().end())
		throw std::runtime_error(directive + "[Directive] unknown");
	WebservConfLevel	valLevels = _getParseMap().find(directive)->second.first;
	while (((lowVallevel << 1) & valLevels) != static_cast<WebservConfLevel>(0)) {
		lowVallevel = lowVallevel << 1;
        if ((lowVallevel & WebservConfLevel::LOCATION)
            != static_cast<WebservConfLevel>(0)) break;
    }
	return lowVallevel;
}

HttpConf*	AWebservParser::getHttpConfPtr(const ConfCtx& confCtx) {
    if (confCtx.httpConfs == nullptr
		|| confCtx.httpConfs->size() <= static_cast<size_t>(_ctxIndex))
		return nullptr;
	return (*confCtx.httpConfs)[_ctxIndex].get();
}

SrvConf*	AWebservParser::getSrvConfPtr(const ConfCtx& confCtx) {
    if (confCtx.srvConfs == nullptr
		|| confCtx.srvConfs->size() <= static_cast<size_t>(_ctxIndex))
		return nullptr;
	return (*confCtx.srvConfs)[_ctxIndex].get();
}

LocConf*	AWebservParser::getLocConfPtr(const ConfCtx& confCtx) {
    if (confCtx.locConfs == nullptr
		|| confCtx.locConfs->size() <= static_cast<size_t>(_ctxIndex))
		return nullptr;
	return (*confCtx.locConfs)[_ctxIndex].get();
}

void AWebservParser::parseDirective(
	ConfigParser& parser,
	WebservConfLevel level) {
    const ConfCtx&	confCtx = parser.getConfCtx();
    Tokens&			tokens = parser.getTokens();
	std::string		directive = tokens.front();

	tokens.pop_front();
	if (tokens.empty() || _isDelimiter(tokens.front()))
		throw std::runtime_error(
			"Invalid definition for directive '" + directive + "'");
	_getParseMap().at(directive).second(
        directive, tokens, confCtx, level, parser);
	if (tokens.empty() || tokens.front() != ";")
		throw std::runtime_error(
			"Invalid definition for directive '" + directive + "'");
	tokens.pop_front();
};

bool	AWebservParser::_isDelimiter(const std::string& tok) {
	return (tok == ";" || tok == "{" || tok == "}");
};

bool	AWebservParser::_parseBooleanValue(const std::string& tok) {
	 if (tok == "on" || tok == "true" || tok == "1")
        return true;
    else if (tok == "off" || tok == "false" || tok == "0")
        return false;
    else
        throw std::runtime_error(
			"Only on/off, true/false, 1/0 are accepted as boolean values");
};

void	AWebservParser::_addLowerLevelDirective(const std::string& directive,
    Tokens vals, Tokens& arr) {
	arr.push_back(directive);
	arr.insert(arr.end(), vals.begin(), vals.end());
    arr.push_back(";");
}

Tokens	AWebservParser::_getDirectiveVals(Tokens& tokens) {
	Tokens	values;
	while (1) {
		values.push_back(tokens.front());
		tokens.pop_front();
        if (tokens.empty() || _isDelimiter(tokens.front()))
            break;
	}
	return values;
}
