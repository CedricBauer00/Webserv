#include "../inc/Configparsing/WebservRedirectParser.hpp"

WebservRedirectParser::WebservRedirectParser(int& ctxIndex) :
	AWebservParser(
		ctxIndex) {
}

const std::unordered_map<
std::string,
std::pair<WebservConfLevel, AWebservParser::parseFunc>>&	WebservRedirectParser::_getParseMap() {
	return _parseMap;
};

void WebservRedirectParser::_parseReturn(Tokens& t, const ConfCtx& c) {
	unsigned long	num;

	if (!isDigits(t.front()))
		throw std::runtime_error("Invalid 'return' directive");

	num = std::stoul(t.front());
	if (statusCodeToReasonPhrase.find(num) == statusCodeToReasonPhrase.end())
		throw std::runtime_error("Invalid 'return' directive");
	t.pop_front();

	if (!t.empty() && !_isDelimiter(t.front())) {
		_assignIfHasNoValue(
			dynamic_cast<LocRedirectConf*>(getLocConfPtr(c))->retDirective,
			{num, t.front()});
		t.pop_front();
	}
	else
		_assignIfHasNoValue(
			dynamic_cast<LocRedirectConf*>(getLocConfPtr(c))->retDirective,
			{num, {}});
}
