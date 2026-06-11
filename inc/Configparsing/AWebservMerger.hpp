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
		void	mergeFromHttpConf(ConfigParser& parser);
};

template<typename T>
void	AWebservMerger::mergeFromHttpConf(ConfigParser& parser) {
	WebservConfLevel	level;
	const ConfCtx&		c = parser.getConfCtx();
	HttpConf*			httpConfPtr = getHttpConfPtr(c);
	Tokens&				tokens = parser.getTokens();

	if (httpConfPtr == nullptr)
		return;
	parser.setTokens(dynamic_cast<T*>(httpConfPtr)->lowerLevelDirectives);
	while (!tokens.empty()) {
		level = getLowestValidLevelOfDirective(tokens.front());
		initConfIfEmptyAtLevel(c, level);
		parseDirective(parser, level);
	}
}
