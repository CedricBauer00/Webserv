#pragma once

#include <unordered_set>
#include <utility>
#include "AWebservParser.hpp"
#include "ConfigParser.hpp"

class WebservCoreParser : public AWebservParser {
    public:
        WebservCoreParser() = delete;
		WebservCoreParser(int& ctxIndex);
        virtual ~WebservCoreParser() = default;

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
