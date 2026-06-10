#include "../../inc/Configparsing/WebservCoreMerger.hpp"
#include "../../inc/Configparsing/WebservCoreParser.hpp"

WebservCoreMerger::WebservCoreMerger() {
}

WebservCoreMerger::~WebservCoreMerger() {
}

void	WebservCoreMerger::mergeConfs(ConfigParser& parser,
	std::unique_ptr<LocNode>& location) {
	const ConfCtx& c = parser.getConfCtx();
	for (const auto& tokens :
		dynamic_cast<WebservCoreParser::HttpCoreConf*>(
			getHttpConfPtr(c))->lowerLevelDirectives) {
		for (const auto& token : tokens)
				std::cout << token << " ";
		std::cout << std::endl;
	}
	(void)location;
}