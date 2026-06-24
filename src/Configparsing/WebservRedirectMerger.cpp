#include "../inc/Configparsing/WebservRedirectMerger.hpp"

WebservRedirectMerger::WebservRedirectMerger() {
}

WebservRedirectMerger::~WebservRedirectMerger() {
}

void	WebservRedirectMerger::printLocConfs(
	const LocNode* node, ConfCtx ctx) {
	if (getLocConfPtr(ctx) != nullptr)
		printLocConf(*dynamic_cast<const LocRedirectConf*>(getLocConfPtr(ctx)));
	for (auto& loc : node->locations)
		printLocConfs(loc.get(),
			{ctx.httpConfs, ctx.srvConfs, &loc.get()->locConfs});
}

void	WebservRedirectMerger::print(const ConfCtx& ctx, const LocNode* node) {
	if (getSrvConfPtr(ctx) != nullptr)
		printSrvConf(*dynamic_cast<const SrvRedirectConf*>(getSrvConfPtr(ctx)));
	printLocConfs(node, ctx);
}

void	WebservRedirectMerger::printSrvConf(const SrvRedirectConf& srvConf) {
	std::cout << "----------SrvRedirectConf----------" << std::endl;
	(void)srvConf;
}

void	WebservRedirectMerger::printLocConf(const LocRedirectConf& locConf) {
	std::cout << "----------LocRedirectConf----------" << std::endl;
	std::cout << "return: "
		<< (locConf.retDirective.has_value()
			? std::to_string(locConf.retDirective.value().statusCode)
				+ " " + locConf.retDirective.value().path
			: "nullopt")
		<< " ; " << std::endl;
}
