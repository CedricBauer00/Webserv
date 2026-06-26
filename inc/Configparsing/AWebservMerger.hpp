#pragma once

#include "IWebservModule.hpp"
#include "ConfigParser.hpp"

class	AWebservMerger : virtual public IWebservModule{
	public:
		AWebservMerger() = default;
		virtual ~AWebservMerger() = default;

		void	inheritFromHttpConf(ConfigParser& parser);
		void	assignSrvDefaults(const ConfCtx& ctx,
			const SrvConf& defaultSrvConf);
		void	assignLocDefaults(const ConfCtx& ctx,
			const LocConf& defaultLocConf);
		void	mergeLocConfs(const LocNode* node, ConfCtx ctx,
            LocConf* parentLocConf, const LocConf& defaultLocConf);
		void	mergeConfs(ConfigParser& parser) override;

		virtual void	print(const ConfCtx& ctx, const LocNode* node) = 0;
};
