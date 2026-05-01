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

typedef struct s_webserv_loc_tree_node{
	std::vector<struct s_webserv_loc_tree_node*>	children;
	struct s_webserv_loc_tree_node*					parent;
	t_webserv_loc_conf								conf;
} t_webserv_loc_tree_node;

typedef unsigned int(*t_webserv_handler_pt)(t_webserv_req* r);

typedef struct {
	std::string name;
	std::string level; // debug, info, notice, warn, error, crit
	int	fd;
} t_webserv_error_log; 

typedef struct {
	std::string	name;
	int			matchType; // 0: exact, 1:normal prefix, 2: prefix, 3: regex

	t_webserv_loc_tree_node*			staticLocations;
	std::vector<t_webserv_loc_conf*>	regexLocations;

	void**	loc_conf;

	unsigned int			allowedMethods; // bitmask of allowed methods
	t_webserv_handler_pt	handler; // handler for this location
	std::string				root; // root directory for this location
	unsigned int			alias; // length of the location prefix to be replaced by root when serving files
	std::string				postAction; // URI to redirect POST requests to

	long					clientMaxBodySize; // maximum allowed size of client request body in bytes
	long					clientBodyBufferSize; // size of buffer used for reading client request body in bytes

	t_webserv_msec			clientBodyTimeout; // maximum time to wait for client request body in milliseconds (408 Request Timeout)
	t_webserv_msec			sendTimeout; // maximum time to wait for sending response to client in milliseconds (504 Gateway Timeout)

	bool					absoluteRedirect{true}; // whether to use absolute URIs in redirects (e.g., Location header in 301/302 responses)
	bool					logNotFound{true}; // whether to log 404 Not Found errors
	t_webserv_error_log		errorLog;

	bool					chunkedTransferEncoding{false}; // whether to use chunked transfer encoding for responses with unknown content length
}	t_webserv_loc_conf;