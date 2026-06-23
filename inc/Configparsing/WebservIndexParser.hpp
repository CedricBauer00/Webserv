#pragma once

#include <optional>
#include "AWebservParser.hpp"
#include "ConfigParser.hpp"

class	WebservIndexParser : public AWebservParser {
	public:
		WebservIndexParser() = delete;
		WebservIndexParser(int& ctxIndex);
		virtual ~WebservIndexParser() = default;

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