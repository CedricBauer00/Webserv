#pragma once

#include "AWebservParser.hpp"
#include "ConfigParser.hpp"

class	WebservIndexParser : public AWebservParser {
	public:
		struct	HttpIndexConf : HttpConf {
			std::vector<std::vector<std::string>>	lowerLevelDirectives; // directives that can be specified in http block and inherited by all servers and locations, e.g., index, autoindex
		};

		struct	SrvIndexConf : SrvConf {
			std::vector<std::string>	indexFiles; // list of index files to look for when a directory is requested
			bool						autoindex{false}; // whether to generate directory listing if no index file is found
		};

		struct LocIndexConf : LocConf {
			std::vector<std::string>	indexFiles; // list of index files to look for when a directory is requested
			bool						autoindex{false}; // whether to generate directory listing if no index file is found
		};

		WebservIndexParser() = delete;
		WebservIndexParser(int& ctxIndex);
		virtual ~WebservIndexParser() = default;
		void	parseDirective(ConfigParser& parser,
			WebservConfLevel level) override;

	private:
		void	_parseIndex(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
		void	_parseAutoindex(const std::string& directive,
			Tokens& tokens, const ConfCtx& confCtx, WebservConfLevel level);
};