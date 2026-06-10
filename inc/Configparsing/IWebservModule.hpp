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
            LocNode*			parent{nullptr};
            std::string			name;
            int					matchType{3}; // 0: exact, 1: prefix, 2: regex, 3:normal prefix
            VecOfPtrs<LocConf>	locConfs;
            VecOfPtrs<LocNode>	locations;
        };

        struct  Srv {
            VecOfPtrs<SrvConf>			srvConfs;
            std::unique_ptr<LocNode>	location;
            Srv() : location(std::make_unique<IWebservModule::LocNode>()) {
			}
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
        virtual void    mergeConfs(ConfigParser& parser,
            std::unique_ptr<LocNode>& location) = 0;
		virtual HttpConf*	getHttpConfPtr(const ConfCtx& confCtx) = 0;
		virtual SrvConf*	getSrvConfPtr(const ConfCtx& confCtx) = 0;
		virtual LocConf*	getLocConfPtr(const ConfCtx& confCtx) = 0;
};
