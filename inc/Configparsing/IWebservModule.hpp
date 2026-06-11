#pragma once

#include "modules.hpp"
#include "confs.hpp"

class ConfigParser;

class IWebservModule {
	public:
		virtual ~IWebservModule() = default;
        virtual bool				isDirectiveValid(
			const std::string& directive, WebservConfLevel level) = 0;
        virtual WebservConfLevel	getLowestValidLevelOfDirective(
	        const std::string& directive) = 0;
        virtual void				initConfIfEmptyAtLevel(
			const ConfCtx& confCtx, WebservConfLevel level) = 0;
		virtual void				parseDirective(
			ConfigParser& parser, WebservConfLevel level) = 0;
        virtual void    			mergeConfs(
			ConfigParser& parser, std::unique_ptr<LocNode>& location) = 0;
		virtual HttpConf*			getHttpConfPtr(const ConfCtx& confCtx) = 0;
		virtual SrvConf*			getSrvConfPtr(const ConfCtx& confCtx) = 0;
		virtual LocConf*			getLocConfPtr(const ConfCtx& confCtx) = 0;
};
