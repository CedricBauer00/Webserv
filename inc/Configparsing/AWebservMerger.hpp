#pragma once

#include "IWebservModule.hpp"
#include "ConfigParser.hpp"

class	AWebservMerger : virtual public IWebservModule{
	private:
		/* data */
	public:
		AWebservMerger() = default;
		virtual ~AWebservMerger() = default;

		template<typename T>
		void	inheritFromHttpConf(ConfigParser& parser);

		void	assignDefaults(const ConfCtx& ctx,
			const SrvConf& defaultSrvConf,
			const LocConf& defaultLocConf) {
			if (getSrvConfPtr(ctx) != nullptr)
				getSrvConfPtr(ctx)->inheritFrom(defaultSrvConf);
			if (getLocConfPtr(ctx) != nullptr)
				getLocConfPtr(ctx)->inheritFrom(defaultLocConf);
		};
};

template<typename T>
void	AWebservMerger::inheritFromHttpConf(ConfigParser& parser) {
	WebservConfLevel	level;
	const ConfCtx&		c = parser.getConfCtx();
	HttpConf*			httpConfPtr = getHttpConfPtr(c);
	Tokens&				tokens = parser.getTokens();

	if (httpConfPtr == nullptr)
		return;
	parser.setTokens(dynamic_cast<T*>(httpConfPtr)->lowerLevelDirectives);
	while (!tokens.empty()) {
		level = getLowestValidLevelOfDirective(tokens.front());
		initConfIfEmptyAtLevel(c, level, &parser.getLocNode());
		parseDirective(parser, level);
	}
}
