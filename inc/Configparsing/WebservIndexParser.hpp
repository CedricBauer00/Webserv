#pragma once

#include <optional>
#include "AWebservParser.hpp"
#include "ConfigParser.hpp"

class	WebservIndexParser : public AWebservParser {
	public:
		struct	HttpIndexConf : HttpConf {
			std::vector<std::vector<std::string>>	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., index, autoindex
		};

		struct	SrvIndexConf : SrvConf {
		};

		struct LocIndexConf : LocConf {
			std::vector<std::string>	indexFiles; // list of index files to look for when a directory is requested
			std::optional<bool>			autoindex; // whether to generate directory listing if no index file is found
		};

		WebservIndexParser() = delete;
		WebservIndexParser(int& ctxIndex);
		virtual ~WebservIndexParser() = default;
		void	parseDirective(ConfigParser& parser,
			WebservConfLevel level) override;

	private:
		const std::unordered_map<
		std::string, std::pair<WebservConfLevel, parseFunc>
		> _parseMap = {
			{"index",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseIndex(d, t, c, l);
					}
				}
			},
			{"autoindex",
				{WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t,
						const ConfCtx& c, WebservConfLevel l, ConfigParser& p) {
						(void)p;
						_parseAutoindex(d, t, c, l);
					}
				}
			},
		};

		const std::unordered_map<
		std::string,
		std::pair<WebservConfLevel, parseFunc>>&	_getParseMap() override;
		void	_parseIndex(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseAutoindex(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
};