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

        void    print(const ConfCtx& ctx, const LocNode* node);
        void    printHttpConf(const HttpCoreConf& httpConf);
        void    printSrvConf(const SrvCoreConf& srvConf);
        void	printLocConfs(const LocNode* node, ConfCtx ctx);
        void    printLocConf(const LocCoreConf& locConf);
};