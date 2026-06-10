#include "../../inc/Configparsing/WebservIndexMerger.hpp"
#include "../../inc/Configparsing/WebservIndexParser.hpp"

WebservIndexMerger::WebservIndexMerger() {
}

WebservIndexMerger::~WebservIndexMerger() {
}

void	WebservIndexMerger::mergeConfs(ConfigParser& parser,
	std::unique_ptr<LocNode>& location) {
	const ConfCtx& c = parser.getConfCtx();
	for (const auto& tokens :
		dynamic_cast<WebservIndexParser::HttpIndexConf*>(
			getHttpConfPtr(c))->lowerLevelDirectives) {
		for (const auto& token : tokens)
				std::cout << token << " ";
		std::cout << std::endl;
	}
	(void)location;
}
