#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "modules.hpp"

struct WebservLocTreeNode {
	std::vector<struct WebservLocTreeNode*>	children;
	struct WebservLocTreeNode*				parent;
	WebservLocCoreConf						conf;
};

struct WebservPhase {
	// t_webserv_loc_conf*					loc_conf;
	std::vector<WebservHandler>		handlers;
};
