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
		void    mergeConfs(ConfigParser& parser,
			std::unique_ptr<LocNode>& location, ConfCtx& confctx) override;
        void    print(const ConfCtx& ctx);
        void    printHttpConf(const HttpIndexConf& httpConf);
        void    printSrvConf(const SrvIndexConf& srvConf);
        void    printLocConf(const LocIndexConf& locConf);
};
