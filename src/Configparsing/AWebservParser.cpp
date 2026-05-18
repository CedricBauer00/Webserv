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
