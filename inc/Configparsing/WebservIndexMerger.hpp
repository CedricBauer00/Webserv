#pragma once

#include "AWebservMerger.hpp"

class	WebservIndexMerger :
public AWebservMerger,
public HttpIndexConf,
public SrvIndexConf,
public LocIndexConf {
	public:
		WebservIndexMerger();
		virtual ~WebservIndexMerger();

        void    print(ConfCtx ctx, std::unique_ptr<LocNode>& location);
        void    printHttpConf(const HttpIndexConf& httpConf);
        void    printSrvConf(const SrvIndexConf& srvConf);
        void	printLocConfs(ConfCtx ctx, std::unique_ptr<LocNode>& location);
        void    printLocConf(const LocIndexConf& locConf);
};
