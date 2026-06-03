#pragma once

#include "WebservIndexParser.hpp"

class WebservIndexModule : public WebservIndexParser {
	public:
		WebservIndexModule() = delete;
		WebservIndexModule(int& ctxIndex);
		virtual ~WebservIndexModule() = default;
};

