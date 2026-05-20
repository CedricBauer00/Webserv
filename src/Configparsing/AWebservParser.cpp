#include "../../inc/Configparsing/AWebservParser.hpp"

AWebservParser::AWebservParser(
    int& ctxIndex,
	std::unordered_map<std::string, WebservConfLevel> directiveValLevelMap) :
	_ctxIndex(ctxIndex++),
    _directiveValLevelMap(std::move(directiveValLevelMap)) {
};

int AWebservParser::isDirectiveValid(
	const std::string& directive,
	WebservConfLevel level) {
	auto it = _directiveValLevelMap.find(directive);
	return (it != _directiveValLevelMap.end()
		&& (it->second & level) != static_cast<WebservConfLevel>(0));
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
