#pragma once

#include "modules.hpp"
#include "IWebservModule.hpp"
#include "ConfigParser.hpp"

class WebservCoreMerger : virtual public IWebservModule {
	public:
		WebservCoreMerger();
		virtual ~WebservCoreMerger();
		void    mergeConfs(ConfigParser& parser,
			std::unique_ptr<LocNode>& location) override;
};