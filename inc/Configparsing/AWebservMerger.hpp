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

		void	assignDefaults(const ConfCtx& ctx,
			const SrvConf& defaultSrvConf,
			const LocConf& defaultLocConf) {
			if (getSrvConfPtr(ctx) != nullptr)
				getSrvConfPtr(ctx)->inheritFrom(defaultSrvConf);
			if (getLocConfPtr(ctx) != nullptr)
				getLocConfPtr(ctx)->inheritFrom(defaultLocConf);
		};

		void	mergeLocConfs(const LocNode* node,
			ConfCtx confctx, LocConf* parentLocConf) {
			if (parentLocConf != nullptr) { 
				initConfIfEmptyAtLevel(
					confctx, WebservConfLevel::LOCATION, node);
				getLocConfPtr(confctx)->inheritFrom(*parentLocConf);
			}
			parentLocConf = getLocConfPtr(confctx);
			for (auto& loc : node->locations) {
				confctx.locConfs = &loc.get()->locConfs;
				mergeLocConfs(loc.get(), confctx, parentLocConf);
			}
		}

		virtual void	print(const ConfCtx& ctx, const LocNode* node) = 0;

		void	mergeConfs(ConfigParser& parser) override {
			inheritFromHttpConf(parser);
			assignDefaults(parser.getConfCtx(),
				dynamic_cast<const SrvConf&>(*this),
				dynamic_cast<const LocConf&>(*this));
			mergeLocConfs(&parser.getLocNode(), parser.getConfCtx(), nullptr);
			print(parser.getConfCtx(), &parser.getLocNode());
		}
};
