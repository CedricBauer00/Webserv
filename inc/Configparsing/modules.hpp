#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <string>
#include <deque>

constexpr const unsigned int    LISTEN = (1 << 0);
constexpr const unsigned int    DEFAULT_SERVER = (1 << 1);

constexpr const char*   IP = "0.0.0.0";
constexpr const char*   PORT = "80";

enum class WebservConfLevel : uint8_t {
    MAIN = 1 << 0,
	HTTP = 1 << 1,
	SERVER = 1 << 2,
	LOCATION = 1 << 3,
};

constexpr WebservConfLevel operator|(WebservConfLevel a, WebservConfLevel b) {
	return static_cast<WebservConfLevel>(static_cast<uint8_t>(a)
	| static_cast<uint8_t>(b));
}

constexpr WebservConfLevel operator&(WebservConfLevel a, WebservConfLevel b) {
	return static_cast<WebservConfLevel>(static_cast<uint8_t>(a)
	& static_cast<uint8_t>(b));
}

constexpr WebservConfLevel operator<<(WebservConfLevel a, int shift) {
    if (a == WebservConfLevel::LOCATION)
        return WebservConfLevel::LOCATION; // prevent overflow
    return static_cast<WebservConfLevel>(static_cast<uint8_t>(a) << shift);
}

template<typename T>
using VecOfPtrs = std::vector<std::unique_ptr<T>>;

using Tokens = std::deque<std::string>;

using WebservMsec = std::chrono::milliseconds;

struct WebservAddr {
	std::string	ip;
	std::string	port;
};

struct WebservConnection {
	int			fd{-1};
	WebservAddr	addr;
};

struct WebservHttpRequest {
	WebservConnection*									conn;
	std::string											method, uri, httpVersion;
	std::vector<std::pair<std::string, std::string>>	headers;
};

typedef int (*WebservHandler)(WebservHttpRequest* r);

struct WebservErrorLog {
	std::string name;
	std::string level; // debug, info, notice, warn, error, crit
	int	fd;
};

bool	isDigits(const std::string& str);
bool	isValidPort(const std::string& str);
bool 	isValidIpv4(const std::string& address);