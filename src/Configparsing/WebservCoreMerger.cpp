#include "../../inc/Configparsing/WebservCoreMerger.hpp"
#include "../../inc/Configparsing/WebservCoreParser.hpp"

WebservCoreMerger::WebservCoreMerger() {
}

WebservCoreMerger::~WebservCoreMerger() {
}

void	WebservCoreMerger::mergeConfs(ConfigParser& parser,
	std::unique_ptr<LocNode>& location) {
	WebservConfLevel	level;
	const ConfCtx&		c = parser.getConfCtx();

    if (getHttpConfPtr(c) != nullptr) {
		parser.setTokens(
			dynamic_cast<HttpCoreConf*>(getHttpConfPtr(c))->lowerLevelDirectives);
		while (!parser.getTokens().empty()) {
			for (const auto& token : parser.getTokens())
					std::cout << token << " ";
			std::cout << std::endl;
			level = getLowestValidLevelOfDirective(parser.getTokens().front());
			std::cout << "lowest valid level of directive: "
			<< std::to_string(static_cast<uint8_t>(level)) << std::endl;
			initConfIfEmptyAtLevel(c, level);
			parseDirective(parser, level);
		}
		// for (const auto& tokens : dynamic_cast<HttpCoreConf*>(
		// 	getHttpConfPtr(c))->lowerLevelDirectives) {
		// 	for (const auto& token : tokens)
		// 			std::cout << token << " ";
		// 	std::cout << std::endl;
		// 	std::cout << "lowest valid level of directive: "
		// 	<< std::to_string(static_cast<uint8_t>(getLowestValidLevelOfDirective(tokens.front())))
		// 	<< std::endl;
		// 	WebservConfLevel	level = getLowestValidLevelOfDirective(tokens.front());
		// 	initConfIfEmptyAtLevel(c, level);
		// 	// parseDirective(parser, level);
		// }
	}
	(void)location;
}