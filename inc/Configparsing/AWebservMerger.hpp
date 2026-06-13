#pragma once

#include "IWebservModule.hpp"
#include "ConfigParser.hpp"

class	AWebservMerger : virtual public IWebservModule{
	private:
		/* data */
	public:
		AWebservMerger() = default;
		virtual ~AWebservMerger() = default;

		void	inheritFromHttpConf(ConfigParser& parser,
			ConfCtx& ctx, HttpConf* httpConfPtr) {
			WebservConfLevel	level;
			Tokens&				tokens = parser.getTokens();

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

		void	mergeLocConfs(std::unique_ptr<LocNode>& location,
			ConfCtx confctx, LocConf* parentLocConf) {
			if (parentLocConf != nullptr) {
				initConfIfEmptyAtLevel(
					confctx, WebservConfLevel::LOCATION, location.get());
				getLocConfPtr(confctx)->inheritFrom(*parentLocConf);
			}
			parentLocConf = getLocConfPtr(confctx);
			for (auto& loc : location.get()->locations) {
				confctx.locConfs = &loc.get()->locConfs;
				mergeLocConfs(loc, confctx, parentLocConf);
			}
		}

		virtual void	print(ConfCtx ctx, std::unique_ptr<LocNode>& location) = 0;

		void	mergeConfs(ConfigParser& parser,
			std::unique_ptr<LocNode>& location, ConfCtx& confctx) override {
			inheritFromHttpConf(parser, confctx, getHttpConfPtr(confctx));
			assignDefaults(confctx, dynamic_cast<const SrvConf&>(*this),
				dynamic_cast<const LocConf&>(*this));
			mergeLocConfs(location, confctx, nullptr);
			print(confctx, location);
		}
};
