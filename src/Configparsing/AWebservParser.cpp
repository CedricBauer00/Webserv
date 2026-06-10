#include "../../inc/Configparsing/AWebservParser.hpp"

AWebservParser::AWebservParser(
    int& ctxIndex) :
	_ctxIndex(ctxIndex++) {
};

int AWebservParser::isDirectiveValid(
	const std::string& directive,
	WebservConfLevel level) {
	auto&	parseMap =  _getParseMap();
	auto it = parseMap.find(directive);
	return (it != parseMap.end()
		&& (it->second.first & level) != static_cast<WebservConfLevel>(0));
};

IWebservModule::HttpConf*	AWebservParser::getHttpConfPtr(const ConfCtx& confCtx) {
	return (*confCtx.httpConfs)[_ctxIndex].get();
}

IWebservModule::SrvConf*	AWebservParser::getSrvConfPtr(const ConfCtx& confCtx) {
	return (*confCtx.srvConfs)[_ctxIndex].get();
}

IWebservModule::LocConf*	AWebservParser::getLocConfPtr(const ConfCtx& confCtx) {
	return (*confCtx.locConfs)[_ctxIndex].get();
}

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

void	AWebservParser::_addLowerLevelDirective(
	const std::string& directive,
    std::vector<std::string> vals,
    std::vector<std::vector<std::string>>& arr) {
    vals.insert(vals.begin(), directive);
    arr.push_back(vals);
}