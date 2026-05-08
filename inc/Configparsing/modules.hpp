#pragma once

#include <queue>
#include <unordered_map>

enum class WebservConfLevel : uint8_t {
	HTTP = 1 << 0,
	SERVER = 1 << 1,
	LOCATION = 1 << 2,
};

constexpr WebservConfLevel operator|(WebservConfLevel a, WebservConfLevel b) {
	return static_cast<WebservConfLevel>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr WebservConfLevel operator&(WebservConfLevel a, WebservConfLevel b) {
	return static_cast<WebservConfLevel>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

class IWebservModule {
	public:
		virtual ~IWebservModule() = default;
		virtual void parseConfig(std::queue<std::string>& tokens, WebservConfLevel level) = 0;
};

class WebservCoreModule : public IWebservModule {
	private:
		const std::unordered_map<std::string, WebservConfLevel> _directiveValidLevels = {
			{"server", WebservConfLevel::HTTP},
			{"listen", WebservConfLevel::SERVER},
			{"server_name", WebservConfLevel::SERVER},
			{"num_req_expected", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"client_header_timeout", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"ignore_invalid_headers", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"merge_slashes", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"underscore_in_headers", WebservConfLevel::HTTP | WebservConfLevel::SERVER},
			{"root", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"allow", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"alias", WebservConfLevel::LOCATION},
			{"client_body_buffer_size", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"client_body_timeout", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"client_max_body_size", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"send_timeout", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"absolute_redirect", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"log_not_found", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"error_page", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"index", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"autoindex", WebservConfLevel::HTTP | WebservConfLevel::SERVER | WebservConfLevel::LOCATION},
			{"try_files", WebservConfLevel::LOCATION},
		};
	public:
		void parseConfig(std::queue<std::string>& tokens, WebservConfLevel level) override;
};