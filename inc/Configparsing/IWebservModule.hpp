#pragma once

#include "modules.hpp"

class ConfigParser;

class IWebservModule {
	public:
        struct HttpConf {
            virtual ~HttpConf() = default;
        };

        struct SrvConf {
            virtual ~SrvConf() = default;
        };

        struct LocConf {
            virtual ~LocConf() = default;
        };

        struct  LocNode {
            LocNode*				parent{nullptr};
            VecOfPtrs<LocConf>		locConfs;
            std::vector<LocNode>	locations;
        };

        struct  SrvNode {
            VecOfPtrs<SrvConf>		srvConfs;
            LocNode					location;
        };

        struct	ConfCtx {
            VecOfPtrs<HttpConf>*	httpConfs{nullptr};
            VecOfPtrs<SrvConf>*		srvConfs{nullptr};
            VecOfPtrs<LocConf>*		locConfs{nullptr};
        };

		virtual ~IWebservModule() = default;
        virtual int		isDirectiveValid(const std::string& directive,
			WebservConfLevel level) = 0;
		virtual void	parseDirective(ConfigParser& parser,
			WebservConfLevel level) = 0;
};
