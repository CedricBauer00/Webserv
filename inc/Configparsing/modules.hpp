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

using Tokens = std::deque<std::string>;

using WebservMsec = std::chrono::milliseconds;

struct HttpConf {
	virtual ~HttpConf() = default;
	virtual Tokens	getlowerLevelDirectives() = 0;
	// iheritFrom() not defined as currently unnecessary
};

struct SrvConf {
	virtual ~SrvConf() = default;
    virtual void	inheritFrom(const SrvConf& other) = 0;
};

struct LocConf {
	virtual ~LocConf() = default;
	virtual void	inheritFrom(const LocConf& other) = 0;
};

template<typename T>
using VecOfPtrs = std::vector<std::unique_ptr<T>>;

struct  LocNode {
	LocNode*			parent{nullptr};
	std::string			name;
	int					matchType{3}; // 0: exact, 1: prefix, 2: regex, 3:normal prefix
	VecOfPtrs<LocConf>	locConfs;
	VecOfPtrs<LocNode>	locations;
};

struct  Srv {
	VecOfPtrs<SrvConf>			srvConfs;
	std::unique_ptr<LocNode>	location;
	Srv() : location(std::make_unique<LocNode>()) {
	}
};

struct	ConfCtx {
	VecOfPtrs<HttpConf>*	httpConfs{nullptr};
	VecOfPtrs<SrvConf>*		srvConfs{nullptr};
	VecOfPtrs<LocConf>*		locConfs{nullptr};
};

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

struct	ErrorPage {
	unsigned long	resCode;
	std::string		path;
};

bool	isDigits(const std::string& str);
bool	isValidPort(const std::string& str);
bool 	isValidIpv4(const std::string& address);