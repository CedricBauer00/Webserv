#pragma once

#include "WebservIndexParser.hpp"
#include "WebservIndexMerger.hpp"

class WebservIndexModule : public WebservIndexParser, public WebservIndexMerger {
	public:
		WebservIndexModule() = delete;
		WebservIndexModule(int& ctxIndex);
		virtual ~WebservIndexModule() = default;

		void	initConfIfEmptyAtLevel(const ConfCtx& confCtx,
			WebservConfLevel level, const LocNode* locNodePtr) override;
};

