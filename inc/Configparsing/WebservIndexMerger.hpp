#pragma once

#include "modules.hpp"
#include "IWebservModule.hpp"
#include "ConfigParser.hpp"

class	WebservIndexMerger : virtual public IWebservModule {
	public:
		WebservIndexMerger();
		virtual ~WebservIndexMerger();
		void    mergeConfs(ConfigParser& parser,
			std::unique_ptr<LocNode>& location) override;
};
