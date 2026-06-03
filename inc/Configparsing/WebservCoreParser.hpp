#pragma once

#include <unordered_set>
#include "AWebservParser.hpp"
#include "ConfigParser.hpp"

class WebservCoreParser : public AWebservParser {
    public:
        struct HttpCoreConf : HttpConf {
            std::vector<std::vector<std::string>>	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., error_log, client_max_body_size
            // std::vector<std::pair<WebservAddr, t_webserv_phase_engine>> ph;
        };

		struct SrvCoreConf : SrvConf {
			std::unordered_set<std::string>	serverNames; // virtual server name entries
			// std::string					filename, serverName;
			// unsigned int					lineNum;
			unsigned long					numReqExpected{1000}; // number of simultaneous requests expected
			WebservMsec						clientHeaderTimeout{1000}; // maximum time to wait for client request headers in milliseconds (408 Request Timeout)
			bool							ignore_invalid_headers{true}, \
			merge_slashes{true}, underscore_is_valid{false};
			unsigned int					flags{0};
		};

		struct LocCoreConf : LocConf {
			// std::vector<WebservLocCoreConf> 	rawlocations;
			// WebservLocTreeNode*					staticLocations;
			// std::vector<WebservLocCoreConf*>	regexLocations;

			// WebservPhase			phases[10];
			unsigned int			allowedMethods{11}; // bitmask of allowed methods
			WebservHandler			handler; // handler for this location
			std::string				root; // root directory for this location
			size_t					alias{0}; // length of the location prefix to be replaced by root when serving files
			std::string				postAction; // URI to redirect POST requests to

			unsigned long			clientMaxBodySize; // maximum allowed size of client request body in bytes
			unsigned long			clientBodyBufferSize; // size of buffer used for reading client request body in bytes

			WebservMsec				clientBodyTimeout; // maximum time to wait for client request body in milliseconds (408 Request Timeout)
			WebservMsec				sendTimeout; // maximum time to wait for sending response to client in milliseconds (504 Gateway Timeout)

			bool					absoluteRedirect{true}; // whether to use absolute URIs in redirects (e.g., Location header in 301/302 responses)
			bool					logNotFound{true}; // whether to log 404 Not Found errors
			WebservErrorLog			errorLog;

			bool					chunkedTransferEncoding{false}; // whether to use chunked transfer encoding for responses with unknown content length
		};

        WebservCoreParser() = delete;
		WebservCoreParser(int& ctxIndex);
        virtual ~WebservCoreParser() = default;
        void	parseDirective(ConfigParser& parser,
			WebservConfLevel level) override;

	private:
		// void	_initConfIfEmptyAtLevel(
		// 	const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseListen(Tokens& tokens, const ConfCtx& confCtx,
			ConfigParser& parser);
		void	_parseServerNames(Tokens& tokens, const ConfCtx& confCtx);
		void	_parseNumReqExpected(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseClientHeaderTimeout(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseBoolDirective(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseRoot(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseAllow(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseAlias(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level,
            ConfigParser& parser);
		void	_parseClientBodyBufferSize(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseClientBodyTimeout(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseClientMaxBodySize(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseSendTimeout(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseErrorPage(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseTryFiles(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
};
