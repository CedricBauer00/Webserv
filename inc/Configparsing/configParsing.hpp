#pragma once

#include <string>
#include <vector>
#include <variant>
#include <chrono>

#define LISTEN (1 << 0)
#define DEFAULT_SERVER (1 << 1)

struct WebservConnection {
	int									fd;
	std::string							ip;
	unsigned int						port;
};

struct WebservHttpRequest {
	WebservConnection*									conn;
	std::string											method, uri, httpVersion;
	std::vector<std::pair<std::string, std::string>>	headers;
};

typedef int (*WebservHandler)(WebservHttpRequest* r);

struct WebservHttpConf;
struct WebservSrvConf;
struct WebservLocConf;

struct WebservCtx{
	std::vector<std::variant<WebservHttpConf>> http_conf;
	std::vector<std::variant<WebservSrvConf>> srv_conf;
	std::vector<std::variant<WebservLocConf>> loc_conf;
};

struct WebservAddr {
	std::string	ip;
	std::string	port;
};

typedef std::chrono::milliseconds	WebservMsec;

struct WebservHttpConf{
	std::vector<WebservSrvConf>   servers; // virtual servers
	std::vector<std::pair<std::string, std::string>>	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., error_log, client_max_body_size
	// std::vector<std::pair<WebservAddr, t_webserv_phase_engine>> ph;
};

struct WebservSrvConf {
	WebservCtx*					        ctx;
	std::vector<std::string>			serverNames; // virtual server name entries
	// std::string							filename, serverName;
	// unsigned int						lineNum;
	size_t								numReqExpected{1000}; // number of simultaneous requests expected
	WebservMsec							clientHeaderTimeout{1000}; // maximum time to wait for client request headers in milliseconds (408 Request Timeout)
	bool								ignore_invalid_headers{true}, \
	merge_slashes{true}, underscore_is_valid{false};
	unsigned int						flags{0};
	WebservLocConf						locations; // default location
};

struct WebservLocTreeNode {
	std::vector<struct WebservLocTreeNode*>	children;
	struct WebservLocTreeNode*				parent;
	WebservLocConf							conf;
};

struct WebservPhase {
	// t_webserv_loc_conf*					loc_conf;
	std::vector<WebservHandler>		handlers;
};

struct WebservErrorLog {
	std::string name;
	std::string level; // debug, info, notice, warn, error, crit
	int	fd;
}; 

struct WebservLocConf {
	std::string	name;
	int			matchType; // 0: exact, 1:normal prefix, 2: prefix, 3: regex

	WebservLocTreeNode*				staticLocations;
	std::vector<WebservLocConf>	regexLocations;
	std::vector<WebservLocConf>

	void**	loc_conf;

	WebservPhase			phases[10];
	unsigned int			allowedMethods; // bitmask of allowed methods
	WebservHandler			handler; // handler for this location
	std::string				root; // root directory for this location
	unsigned int			alias; // length of the location prefix to be replaced by root when serving files
	std::string				postAction; // URI to redirect POST requests to

	long					clientMaxBodySize; // maximum allowed size of client request body in bytes
	long					clientBodyBufferSize; // size of buffer used for reading client request body in bytes

	WebservMsec				clientBodyTimeout; // maximum time to wait for client request body in milliseconds (408 Request Timeout)
	WebservMsec				sendTimeout; // maximum time to wait for sending response to client in milliseconds (504 Gateway Timeout)

	bool					absoluteRedirect{true}; // whether to use absolute URIs in redirects (e.g., Location header in 301/302 responses)
	bool					logNotFound{true}; // whether to log 404 Not Found errors
	WebservErrorLog			errorLog;

	bool					chunkedTransferEncoding{false}; // whether to use chunked transfer encoding for responses with unknown content length
};
