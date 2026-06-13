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

        void    print(ConfCtx ctx, std::unique_ptr<LocNode>& location);
        void    printHttpConf(const HttpCoreConf& httpConf);
        void    printSrvConf(const SrvCoreConf& srvConf);
        void	printLocConfs(ConfCtx ctx, std::unique_ptr<LocNode>& location);
        void    printLocConf(const LocCoreConf& locConf);
};