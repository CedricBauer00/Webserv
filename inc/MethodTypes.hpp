#pragma once

#include <unordered_map>

enum method
{
    METHOD_GET,
    METHOD_POST,
    METHOD_DELETE
};

inline const std::unordered_map<std::string,
	std::pair<unsigned int, method>> methodMap = {
	{"GET", {1u<<0, METHOD_GET}},
	{"POST", {1u<<1, METHOD_POST}},
	{"DELETE", {1u<<2, METHOD_DELETE}},
	// {"PUT", 1u<<3},
	// {"HEAD", 1u<<4},
	// {"OPTIONS", 1u<<5}
};
