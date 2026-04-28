#pragma once

#include <string>
#include <vector>
#include <chrono>

#define LISTEN (1 << 0)
#define DEFAULT_SERVER (1 << 1)

typedef struct s_webserrv_connection {
	int									fd;
	std::string							ip;
	unsigned int						port;
} t_webserv_connection;

typedef struct s_webserv_http_request {
	t_webserv_connection*					conn;
	std::string								method, uri, httpVersion;
	std::vector<std::pair<std::string, std::string>> headers;
}	t_webserv_http_request;

typedef int (*t_webserv_handler)(t_webserv_http_request* r);

typedef struct {
	void**	main_conf;
	void**	srv_conf;
	void**	loc_conf;
}	t_webserv_conf_ctx;

typedef struct {
	std::string		domain;
	unsigned int	port;
}	t_webserv_addr;

typedef std::chrono::milliseconds	t_webserv_msec;

typedef struct s_webserv_http_conf	t_webserv_http_conf;
typedef struct s_webserv_srv_conf	t_webserv_srv_conf;
typedef struct s_webserv_loc_conf	t_webserv_loc_conf;

typedef struct {
	std::vector<t_webserv_srv_conf>   servers; // virtual servers
	std::vector<std::pair<t_webserv_addr, t_webserv_phase_engine>> ph;
}	t_webserv_http_conf;

typedef struct {
	t_webserv_conf_ctx*					ctx;
	t_webserv_phase						phases[10];
	std::vector<std::string>			serverNames; // virtual server name entries
	std::string							filename, serverName;
	unsigned int						lineNum;
	size_t								numReqExpected; // number of simultaneous requests expected
	t_webserv_msec						clientHeaderTimeout;
	bool								ignore_invalid_headers{true}, \
	merge_slashes{true}, underscore_is_valid{false};
	unsigned int						flags{0};
}	t_webserv_srv_conf;

