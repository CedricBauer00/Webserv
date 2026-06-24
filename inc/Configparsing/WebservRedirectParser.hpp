#pragma once

#include "AWebservParser.hpp"
#include "../../inc/statusCodes.hpp"

class WebservRedirectParser : public AWebservParser {
	public:
		WebservRedirectParser() = delete;
		WebservRedirectParser(int& ctxIndex);
		virtual ~WebservRedirectParser() = default;

	private:
		const std::unordered_map<
		std::string, std::pair<WebservConfLevel, parseFunc>
		> _parseMap = {
			{"return",
				{WebservConfLevel::SERVER | WebservConfLevel::LOCATION,
					[this](const std::string& d, Tokens& t, const ConfCtx& c,
						WebservConfLevel l, ConfigParser& p) {
							(void)d; (void)l; (void)p;
							_parseReturn(t, c);
					}
				}
			},
		};

		const std::unordered_map<
		std::string,
		std::pair<WebservConfLevel, parseFunc>>&	_getParseMap() override;

		void	_parseReturn(Tokens& tokens, const ConfCtx& confCtx);
};
