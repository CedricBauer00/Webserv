#pragma once

#include <optional>
#include <vector>
#include <unordered_set>
#include "modules.hpp"

struct HttpCoreConf : HttpConf {
	Tokens	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., error_log, client_max_body_size
	// std::vector<std::pair<WebservAddr, t_webserv_phase_engine>> ph;
};

struct SrvCoreConf : SrvConf {
	std::unordered_set<std::string>	serverNames; // virtual server name entries
	// std::string					filename, serverName;
	// unsigned int					lineNum;
	std::optional<unsigned long>	numReqExpected; // number of simultaneous requests expected
	std::optional<WebservMsec>		clientHeaderTimeout; // maximum time to wait for client request headers in milliseconds (408 Request Timeout)
	std::optional<bool>				ignore_invalid_headers,\
	merge_slashes, underscore_is_valid;
	unsigned int					flags{0};

	SrvCoreConf() = default;
	SrvCoreConf(std::unordered_set<std::string> _serverNames,
		std::optional<unsigned long> _numReqExpected,
		std::optional<WebservMsec> _clientHeaderTimeout,
		std::optional<bool> _ignore_invalid_headers,
		std::optional<bool> _merge_slashes,
		std::optional<bool> _underscore_is_valid)
		: 
		serverNames(std::move(_serverNames)),
		numReqExpected(_numReqExpected),
		clientHeaderTimeout(_clientHeaderTimeout),
		ignore_invalid_headers(_ignore_invalid_headers),
		merge_slashes(_merge_slashes),
		underscore_is_valid(_underscore_is_valid)
	{}
};

struct LocCoreConf : LocConf {
	// std::vector<WebservLocCoreConf> 	rawlocations;
	// WebservLocTreeNode*					staticLocations;
	// std::vector<WebservLocCoreConf*>	regexLocations;

	// WebservPhase			phases[10];
	std::optional<unsigned int>		allowedMethods; // bitmask of allowed methods {11}
	// WebservHandler			handler; // handler for this location
	std::string						root; // root directory for this location
	std::optional<size_t>			alias; // length of the location prefix to be replaced by root when serving files
	std::string						postRedirect; // URI to redirect POST requests to

	std::optional<unsigned long>	clientMaxBodySize; // maximum allowed size of client request body in bytes
	std::optional<unsigned long>	clientBodyBufferSize; // size of buffer used for reading client request body in bytes

	std::optional<WebservMsec>		clientBodyTimeout; // maximum time to wait for client request body in milliseconds (408 Request Timeout)
	std::optional<WebservMsec>		sendTimeout; // maximum time to wait for sending response to client in milliseconds (504 Gateway Timeout)

	std::optional<bool>				absoluteRedirect; // whether to use absolute URIs in redirects (e.g., Location header in 301/302 responses)
	std::optional<bool>				logNotFound; // whether to log 404 Not Found errors
	// WebservErrorLog			errorLog;

	std::optional<bool>				chunkedTransferEncoding; // whether to use chunked transfer encoding for responses with unknown content length

	LocCoreConf() = default;
	LocCoreConf(std::optional<unsigned int> _allowedMethods,
		std::string _root,
		std::optional<size_t> _alias,
		std::string _postRedirect,
		std::optional<unsigned long> _clientMaxBodySize,
		std::optional<unsigned long> _clientBodyBufferSize,
		std::optional<WebservMsec> _clientBodyTimeout,
		std::optional<WebservMsec> _sendTimeout,
		std::optional<bool> _absoluteRedirect,
		std::optional<bool> _logNotFound,
		std::optional<bool> _chunkedTransferEncoding)
		: allowedMethods(_allowedMethods),
		root(std::move(_root)),
		alias(_alias),
		postRedirect(std::move(_postRedirect)),
		clientMaxBodySize(_clientMaxBodySize),
		clientBodyBufferSize(_clientBodyBufferSize),
		clientBodyTimeout(_clientBodyTimeout),
		sendTimeout(_sendTimeout),
		absoluteRedirect(_absoluteRedirect),
		logNotFound(_logNotFound),
		chunkedTransferEncoding(_chunkedTransferEncoding)
	{}
};

struct	HttpIndexConf : HttpConf {
	Tokens	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., index, autoindex
};

struct	SrvIndexConf : SrvConf {
};

struct LocIndexConf : LocConf {
	Tokens				indexFiles; // list of index files to look for when a directory is requested
	std::optional<bool>	autoindex; // whether to generate directory listing if no index file is found

	LocIndexConf() = default;
	LocIndexConf(Tokens _indexFiles, bool _autoindex) :
		indexFiles(std::move(_indexFiles)),
		autoindex(_autoindex)
	{}
};