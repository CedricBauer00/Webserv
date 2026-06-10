#pragma once

#include <unordered_set>
#include <optional>
#include <utility>
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
			std::optional<unsigned long>	numReqExpected; // number of simultaneous requests expected
			std::optional<WebservMsec>		clientHeaderTimeout; // maximum time to wait for client request headers in milliseconds (408 Request Timeout)
			std::optional<bool>				ignore_invalid_headers,\
			merge_slashes, underscore_is_valid;
			unsigned int					flags{0};
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
			std::string						postAction; // URI to redirect POST requests to

			std::optional<unsigned long>	clientMaxBodySize; // maximum allowed size of client request body in bytes
			std::optional<unsigned long>	clientBodyBufferSize; // size of buffer used for reading client request body in bytes

			std::optional<WebservMsec>		clientBodyTimeout; // maximum time to wait for client request body in milliseconds (408 Request Timeout)
			std::optional<WebservMsec>		sendTimeout; // maximum time to wait for sending response to client in milliseconds (504 Gateway Timeout)

			std::optional<bool>				absoluteRedirect; // whether to use absolute URIs in redirects (e.g., Location header in 301/302 responses)
			std::optional<bool>				logNotFound; // whether to log 404 Not Found errors
			// WebservErrorLog			errorLog;

			std::optional<bool>				chunkedTransferEncoding; // whether to use chunked transfer encoding for responses with unknown content length
		};

        WebservCoreParser() = delete;
		WebservCoreParser(int& ctxIndex);
        virtual ~WebservCoreParser() = default;
        void	parseDirective(ConfigParser& parser,
			WebservConfLevel level) override;

	private:
		const std::unordered_map<
		std::string, std::pair<WebservConfLevel, parseFunc>
		> _parseMap = {
			{"listen",
				{WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t, const ConfCtx& c,
						WebservConfLevel l, ConfigParser& p) {
							(void)d; (void)l;
							_parseListen(t, c, p);
						}
				}
			},
			{"server_name",
				{WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t, const ConfCtx& c,
						WebservConfLevel l, ConfigParser& p) {
						(void)d; (void)l; (void)p;
						_parseServerNames(t, c);
						}
				}
			},
			{"num_req_expected",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseNumReqExpected(d, t, c, l);
					}
				}
			},
			{"client_header_timeout",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseClientHeaderTimeout(d, t, c, l);
					}
				}
			},
			{"ignore_invalid_headers",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseIgnoreInvalidHeaders(d, t, c, l);
					}
				}
			},
			{"merge_slashes",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseMergeSlashes(d, t, c, l);
					}
				}
			},
			{"underscore_in_headers",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseUnderscoreInHeaders(d, t, c, l);
					}
				}
			},
			{"root",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseRoot(d, t, c, l);
					}
				}
			},
			{"allow",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseAllow(d, t, c, l);
					}
				}
			},
			{"alias",
				{WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						_parseAlias(d, t, c, l, p);
					}
				}
			},
			{"client_body_buffer_size",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseClientBodyBufferSize(d, t, c, l);
					}
				}
			},
			{"client_body_timeout",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseClientBodyTimeout(d, t, c, l);
					}
				}
			},
			{"client_max_body_size",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseClientMaxBodySize(d, t, c, l);
					}
				}
			},
			{"send_timeout",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseSendTimeout(d, t, c, l);
					}
				}
			},
			{"absolute_redirect",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseAbsoluteRedirect(d, t, c, l);
					}
				}
			},
			{"log_not_found",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseLogNotFound(d, t, c, l);
					}
				}
			},
			{"error_page",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseErrorPage(d, t, c, l);
					}
				}
			},
			{"try_files",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseTryFiles(d, t, c, l);
					}
				}
			},
		};

		template<typename T>
		void	_assignIfHasNoValue(std::optional<T> var, T val) {
			if (!var.has_value()) var = val; 
		};

		const std::unordered_map<
		std::string,
		std::pair<WebservConfLevel, parseFunc>>&	_getParseMap() override;
		void	_parseListen(Tokens& tokens, const ConfCtx& confCtx,
			ConfigParser& parser);
		void	_parseServerNames(Tokens& tokens, const ConfCtx& confCtx);
		void	_parseNumReqExpected(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseClientHeaderTimeout(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseIgnoreInvalidHeaders(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseMergeSlashes(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseUnderscoreInHeaders(const std::string& directive,
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
		void	_parseAbsoluteRedirect(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseLogNotFound(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseErrorPage(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseTryFiles(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
};
