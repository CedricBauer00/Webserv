#pragma once

#include "AWebservMerger.hpp"

class WebservCoreMerger :
public AWebservMerger,
public HttpCoreConf,
public SrvCoreConf,
public LocCoreConf {
	public:
		WebservCoreMerger();
		virtual ~WebservCoreMerger();

        void	mergeLocConfs(std::unique_ptr<LocNode>& location,
            ConfCtx confctx, LocConf* parentLocConf);
		void    mergeConfs(ConfigParser& parser,
			std::unique_ptr<LocNode>& location, ConfCtx& confctx) override;
        void    print(const ConfCtx& ctx);
        void    printHttpConf(const HttpCoreConf& httpConf);
        void    printSrvConf(const SrvCoreConf& srvConf);
        void    printLocConf(const LocCoreConf& locConf);
};