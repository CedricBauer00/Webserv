#pragma once

#include <string>
#include <vector>
#include <chrono>

#define LISTEN (1 << 0)
#define DEFAULT_SERVER (1 << 1)

typedef std::chrono::milliseconds	t_webserv_msec;

typedef t_webserv_srv_config;

typedef struct {
	std::vector<t_webserv_srv_config>   servers; // virtual servers
}	t_webserv_http_config;

typedef	t_webserv_loc_config;

typedef struct {
	t_webserv_http_config				*httpConfig; // http configuration
	std::vector<std::string>			serverNames; // virtual server name entries
	std::vector<t_webserv_loc_config>	locations; // location blocks
	std::string							filename, serverName;
	unsigned int						lineNum;
	size_t								numReqExpected; // number of simultaneous requests expected
	t_webserv_msec						clientHeaderTimeout;
	bool								ignore_invalid_headers{true}, \
	merge_slashes{true}, underscore_is_valid{false};
	unsigned int						flags{0};
}	t_webserv_srv_config;