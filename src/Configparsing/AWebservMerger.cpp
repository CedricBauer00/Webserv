#include "../../inc/Configparsing/AWebservMerger.hpp"

void	AWebservMerger::inheritFromHttpConf(ConfigParser& parser) {
	WebservConfLevel	level;
	Tokens&				tokens = parser.getTokens();
	const ConfCtx&		ctx = parser.getConfCtx();
	HttpConf*			httpConfPtr = getHttpConfPtr(ctx);

	if (httpConfPtr == nullptr)
		return;
	parser.setTokens(httpConfPtr->getlowerLevelDirectives());
	while (!tokens.empty()) {
		level = getLowestValidLevelOfDirective(tokens.front());
		initConfIfEmptyAtLevel(ctx, level, &parser.getLocNode());
		parseDirective(parser, level);
	}
}

void	AWebservMerger::assignSrvDefaults(const ConfCtx& ctx,
	const SrvConf& defaultSrvConf) {
	if (getSrvConfPtr(ctx) != nullptr)
		getSrvConfPtr(ctx)->inheritFrom(defaultSrvConf);
}

void	AWebservMerger::assignLocDefaults(const ConfCtx& ctx,
	const LocConf& defaultLocConf) {
	if (getLocConfPtr(ctx) != nullptr)
		getLocConfPtr(ctx)->inheritFrom(defaultLocConf);
}

void	AWebservMerger::mergeLocConfs(const LocNode* node, ConfCtx ctx,
	LocConf* parentLocConf, const LocConf& defaultLocConf) {
	if (parentLocConf != nullptr) { 
		initConfIfEmptyAtLevel(
			ctx, WebservConfLevel::LOCATION, node);
		getLocConfPtr(ctx)->inheritFrom(*parentLocConf);
	}
	else
		assignLocDefaults(ctx, defaultLocConf);
	parentLocConf = getLocConfPtr(ctx);
	for (auto& loc : node->locations) {
		ctx.locConfs = &loc.get()->locConfs;
		mergeLocConfs(loc.get(), ctx, parentLocConf, defaultLocConf);
	}
}

void	AWebservMerger::mergeConfs(ConfigParser& parser) {
	inheritFromHttpConf(parser);
	assignSrvDefaults(parser.getConfCtx(),
		dynamic_cast<const SrvConf&>(*this));
	mergeLocConfs(&parser.getLocNode(), parser.getConfCtx(), nullptr,
		dynamic_cast<const LocConf&>(*this));
	// print(parser.getConfCtx(), &parser.getLocNode());
}