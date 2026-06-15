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

        void    print(const ConfCtx& ctx, const LocNode* node);
        void    printHttpConf(const HttpIndexConf& httpConf);
        void    printSrvConf(const SrvIndexConf& srvConf);
        void	printLocConfs(const LocNode* node, ConfCtx ctx);
        void    printLocConf(const LocIndexConf& locConf);
};
