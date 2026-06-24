#pragma once

#include "WebservRedirectParser.hpp"
#include "WebservRedirectMerger.hpp"

class WebservRedirectModule : public WebservRedirectParser, public WebservRedirectMerger {
	public:
		WebservRedirectModule() = delete;
		WebservRedirectModule(int& ctxIndex);
		virtual ~WebservRedirectModule() = default;

		void	initConfIfEmptyAtLevel(const ConfCtx& confCtx,
			WebservConfLevel level, const LocNode* locNodePtr) override;
};
