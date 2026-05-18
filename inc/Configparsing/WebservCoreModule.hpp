#pragma once

#include "WebservCoreParser.hpp"

class WebservCoreModule : public WebservCoreParser {
	public:
        WebservCoreModule() = delete;
		WebservCoreModule(int& ctxIndex);
        virtual ~WebservCoreModule() = default;
};
