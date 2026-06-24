#pragma once

#include "AWebservMerger.hpp"

class	WebservRedirectMerger :
public AWebservMerger,
public HttpRedirectConf,
public SrvRedirectConf,
public LocRedirectConf {
	public:
		WebservRedirectMerger();
		virtual ~WebservRedirectMerger();

		void    print(const ConfCtx& ctx, const LocNode* node);
        void    printSrvConf(const SrvRedirectConf& srvConf);
        void	printLocConfs(const LocNode* node, ConfCtx ctx);
        void    printLocConf(const LocRedirectConf& locConf);
};