#include "../../inc/Configparsing/WebservIndexMerger.hpp"
#include "../../inc/Configparsing/WebservIndexParser.hpp"

WebservIndexMerger::WebservIndexMerger() :
HttpIndexConf(),
SrvIndexConf(),
LocIndexConf({}, false) {
}

WebservIndexMerger::~WebservIndexMerger() {
}

void	WebservIndexMerger::printLocConfs(
	const LocNode* node, ConfCtx ctx) {
	if (getLocConfPtr(ctx) != nullptr)
		printLocConf(*dynamic_cast<const LocIndexConf*>(getLocConfPtr(ctx)));
	for (auto& loc : node->locations)
		printLocConfs(loc.get(),
			{ctx.httpConfs, ctx.srvConfs, &loc.get()->locConfs});
}

void	WebservIndexMerger::print(const ConfCtx& ctx, const LocNode* node) {
	if (getHttpConfPtr(ctx) != nullptr)
		printHttpConf(*dynamic_cast<const HttpIndexConf*>(getHttpConfPtr(ctx)));
	if (getSrvConfPtr(ctx) != nullptr)
		printSrvConf(*dynamic_cast<const SrvIndexConf*>(getSrvConfPtr(ctx)));
	printLocConfs(node, ctx);
}

void	WebservIndexMerger::printHttpConf(const HttpIndexConf& httpConf) {
	std::cout << "----------HttpIndexConf----------" << std::endl;
	for (const auto& token : httpConf.lowerLevelDirectives)
		std::cout << token << " ";
	std::cout << std::endl;
}

void	WebservIndexMerger::printSrvConf(const SrvIndexConf& srvConf) {
	std::cout << "----------SrvIndexConf----------" << std::endl;
	(void)srvConf;
}

void	WebservIndexMerger::printLocConf(const LocIndexConf& locConf) {
	std::cout << "----------LocIndexConf----------" << std::endl;
	std::cout << "indexFiles: ";
	for (const auto& file : locConf.indexFiles)
		std::cout << file << " ";
	std::cout << " ; ";
	std::cout << "autoindex: "
		<< (locConf.autoindex.has_value() ? (locConf.autoindex.value() ? "true" : "false") : "nullopt")
		<< " ; " << std::endl;
}