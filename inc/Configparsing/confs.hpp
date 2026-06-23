#pragma once

#include <optional>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "modules.hpp"

struct HttpCoreConf : HttpConf {
	Tokens	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., error_log, client_max_body_size
	virtual ~HttpCoreConf() = default;
	Tokens	getlowerLevelDirectives() override { return lowerLevelDirectives; };
};

struct SrvCoreConf : SrvConf {
	std::unordered_set<std::string>	serverNames; // virtual server name entries
	// std::string					filename, serverName;
	// unsigned int					lineNum;
	std::optional<unsigned long>	numReqExpected; // number of simultaneous requests expected
	std::optional<WebservMsec>		clientHeaderTimeout; // maximum time to wait for client request headers in milliseconds (408 Request Timeout)
	std::optional<bool>				ignore_invalid_headers,\
	merge_slashes, underscore_is_valid;
	std::optional<unsigned long>	flags;

	SrvCoreConf() = default;
	SrvCoreConf(
		std::unordered_set<std::string> _serverNames,
		std::optional<unsigned long> _numReqExpected,
		std::optional<WebservMsec> _clientHeaderTimeout,
		std::optional<bool> _ignore_invalid_headers,
		std::optional<bool> _merge_slashes,
		std::optional<bool> _underscore_is_valid,
		std::optional<unsigned long> _flags)
		: 
		serverNames(std::move(_serverNames)),
		numReqExpected(_numReqExpected),
		clientHeaderTimeout(_clientHeaderTimeout),
		ignore_invalid_headers(_ignore_invalid_headers),
		merge_slashes(_merge_slashes),
		underscore_is_valid(_underscore_is_valid),
		flags(_flags)
	{}
	void	inheritFrom(const SrvConf& other) override {
		auto& conf = dynamic_cast<const SrvCoreConf&>(other);
		if (serverNames.empty() && !conf.serverNames.empty())
			serverNames = conf.serverNames;
		if (!numReqExpected.has_value() && conf.numReqExpected.has_value())
			numReqExpected = conf.numReqExpected;
		if (!clientHeaderTimeout.has_value() && conf.clientHeaderTimeout.has_value())
			clientHeaderTimeout = conf.clientHeaderTimeout;
		if (!ignore_invalid_headers.has_value() && conf.ignore_invalid_headers.has_value())
			ignore_invalid_headers = conf.ignore_invalid_headers;
		if (!merge_slashes.has_value() && conf.merge_slashes.has_value())
			merge_slashes = conf.merge_slashes;
		if (!underscore_is_valid.has_value() && conf.underscore_is_valid.has_value())
			underscore_is_valid = conf.underscore_is_valid;
		if (!flags.has_value() == 0 && conf.flags.has_value())
			flags = conf.flags;
	}
};

struct LocCoreConf : LocConf {
	std::size_t						nameLen{0};
	// std::vector<WebservLocCoreConf> 	rawlocations;
	// WebservLocTreeNode*					staticLocations;
	// std::vector<WebservLocCoreConf*>	regexLocations;

	// WebservPhase			phases[10];
	std::optional<unsigned int>		allowedMethods; // bitmask of allowed methods {11}
	// WebservHandler			handler; // handler for this location
	std::string						root; // root directory for this location
	std::optional<bool>				alias; // when true, location prefix to be replaced by root when serving files
	std::string						postRedirect; // URI to redirect POST requests to

	std::optional<unsigned long>	clientMaxBodySize; // maximum allowed size of client request body in bytes
	std::optional<unsigned long>	clientBodyBufferSize; // size of buffer used for reading client request body in bytes

	std::optional<WebservMsec>		clientBodyTimeout; // maximum time to wait for client request body in milliseconds (408 Request Timeout)
	std::optional<WebservMsec>		sendTimeout; // maximum time to wait for sending response to client in milliseconds (504 Gateway Timeout)

	std::optional<bool>				absoluteRedirect; // whether to use absolute URIs in redirects (e.g., Location header in 301/302 responses)
	std::optional<bool>				logNotFound; // whether to log 404 Not Found errors
	// WebservErrorLog			errorLog;

	std::optional<bool>				chunkedTransferEncoding; // whether to use chunked transfer encoding for responses with unknown content length

	std::unordered_map<unsigned long, ErrorPage>	errPages;

	std::optional<unsigned long>	redirectCode;

	LocCoreConf() = delete;
	LocCoreConf(
		const LocNode* locNodePtr) 
		:
		nameLen(locNodePtr->name.size())
	{}
	LocCoreConf(
		std::optional<unsigned int> _allowedMethods,
		std::string _root,
		std::optional<bool> _alias,
		std::string _postRedirect,
		std::optional<unsigned long> _clientMaxBodySize,
		std::optional<unsigned long> _clientBodyBufferSize,
		std::optional<WebservMsec> _clientBodyTimeout,
		std::optional<WebservMsec> _sendTimeout,
		std::optional<bool> _absoluteRedirect,
		std::optional<bool> _logNotFound,
		std::optional<bool> _chunkedTransferEncoding,
		std::unordered_map<unsigned long, ErrorPage> _errPages,
		std::optional<int>	_redirectCode = std::nullopt) // 301, 302, 303
		:
		allowedMethods(_allowedMethods),
		root(std::move(_root)),
		alias(_alias),
		postRedirect(std::move(_postRedirect)),
		clientMaxBodySize(_clientMaxBodySize),
		clientBodyBufferSize(_clientBodyBufferSize),
		clientBodyTimeout(_clientBodyTimeout),
		sendTimeout(_sendTimeout),
		absoluteRedirect(_absoluteRedirect),
		logNotFound(_logNotFound),
		chunkedTransferEncoding(_chunkedTransferEncoding),
		errPages(std::move(_errPages)),
		redirectCode( _redirectCode )
	{}
	void	inheritFrom(const LocConf& other) override {
		auto& conf = dynamic_cast<const LocCoreConf&>(other);
		if (!allowedMethods.has_value() && conf.allowedMethods.has_value())
			allowedMethods = conf.allowedMethods;
		if (root.empty() && !conf.root.empty())
			root = conf.root;
		if (!alias.has_value() && conf.alias.has_value())
			alias = conf.alias;
		if (postRedirect.empty() && !conf.postRedirect.empty())
			postRedirect = conf.postRedirect;
		if (!clientMaxBodySize.has_value() && conf.clientMaxBodySize.has_value())
			clientMaxBodySize = conf.clientMaxBodySize;
		if (!clientBodyBufferSize.has_value() && conf.clientBodyBufferSize.has_value())
			clientBodyBufferSize = conf.clientBodyBufferSize;
		if (!clientBodyTimeout.has_value() && conf.clientBodyTimeout.has_value())
			clientBodyTimeout = conf.clientBodyTimeout;
		if (!sendTimeout.has_value() && conf.sendTimeout.has_value())
			sendTimeout = conf.sendTimeout;
		if (!absoluteRedirect.has_value() && conf.absoluteRedirect.has_value())
			absoluteRedirect = conf.absoluteRedirect;
		if (!logNotFound.has_value() && conf.logNotFound.has_value())
			logNotFound = conf.logNotFound;
		if (!chunkedTransferEncoding.has_value() && conf.chunkedTransferEncoding.has_value())
			chunkedTransferEncoding = conf.chunkedTransferEncoding;
		if (!redirectCode.has_value() && conf.redirectCode.has_value())
			redirectCode = conf.redirectCode;
		if (!redirectUri.has_value() && conf.redirectUri.has_value())
			redirectUri = conf.redirectUri;
	}
};

struct	HttpIndexConf : HttpConf {
	Tokens	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., index, autoindex
	virtual ~HttpIndexConf() = default;
	Tokens	getlowerLevelDirectives() override { return lowerLevelDirectives; };
};

struct	SrvIndexConf : SrvConf {
	void	inheritFrom(const SrvConf& otherConf) override { (void)otherConf; };
};

struct LocIndexConf : LocConf {
	Tokens				indexFiles; // list of index files to look for when a directory is requested
	std::optional<bool>	autoindex; // whether to generate directory listing if no index file is found

	LocIndexConf() = delete;
	LocIndexConf(
		const LocNode* locNodePtr) 
	{ (void)locNodePtr; }
	LocIndexConf(
		Tokens _indexFiles,
		bool _autoindex)
		:
		indexFiles(std::move(_indexFiles)),
		autoindex(_autoindex)
	{}
	void	inheritFrom(const LocConf& otherConf) override {
		auto& conf = dynamic_cast<const LocIndexConf&>(otherConf);
		if (indexFiles.empty() && !conf.indexFiles.empty())
			indexFiles = conf.indexFiles;
		if (!autoindex.has_value() && conf.autoindex.has_value())
			autoindex = conf.autoindex;
	}
};