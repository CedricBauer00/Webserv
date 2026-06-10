#pragma once

#include "WebservCoreParser.hpp"
#include "WebservCoreMerger.hpp"

class WebservCoreModule : public WebservCoreParser, public WebservCoreMerger {
	public:
        WebservCoreModule() = delete;
		WebservCoreModule(int& ctxIndex);
        virtual ~WebservCoreModule() = default;
};
