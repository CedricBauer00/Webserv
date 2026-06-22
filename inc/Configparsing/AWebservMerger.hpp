#pragma once

#include "IWebservModule.hpp"
#include "ConfigParser.hpp"

class	AWebservMerger : virtual public IWebservModule{
	private:
		/* data */
	public:
		AWebservMerger() = default;
		virtual ~AWebservMerger() = default;

		void	inheritFromHttpConf(ConfigParser& parser) {
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
		};

		void	assignSrvDefaults(const ConfCtx& ctx,
			const SrvConf& defaultSrvConf) {
			if (getSrvConfPtr(ctx) != nullptr)
				getSrvConfPtr(ctx)->inheritFrom(defaultSrvConf);
		};

		void	mergeLocConfs(const LocNode* node, ConfCtx ctx,
            LocConf* parentLocConf, const LocConf& defaultLocConf) {
			if (parentLocConf != nullptr) { 
				initConfIfEmptyAtLevel(
					ctx, WebservConfLevel::LOCATION, node);
				getLocConfPtr(ctx)->inheritFrom(*parentLocConf);
			}
			else {
				if (getLocConfPtr(ctx) != nullptr)
					getLocConfPtr(ctx)->inheritFrom(defaultLocConf);
			}
			parentLocConf = getLocConfPtr(ctx);
			for (auto& loc : node->locations) {
				ctx.locConfs = &loc.get()->locConfs;
				mergeLocConfs(loc.get(), ctx, parentLocConf, defaultLocConf);
			}
		}

		virtual void	print(const ConfCtx& ctx, const LocNode* node) = 0;

		void	mergeConfs(ConfigParser& parser) override {
			inheritFromHttpConf(parser);
			assignSrvDefaults(parser.getConfCtx(),
				dynamic_cast<const SrvConf&>(*this));
			mergeLocConfs(&parser.getLocNode(), parser.getConfCtx(), nullptr,
				dynamic_cast<const LocConf&>(*this));
			// print(parser.getConfCtx(), &parser.getLocNode());
		}
};
