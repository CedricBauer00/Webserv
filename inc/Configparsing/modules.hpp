#pragma once

#include <queue>
#include <unordered_map>

enum class WebservConfLevel : uint8_t {
    MAIN = 1 << 0,
	HTTP = 1 << 1,
	SERVER = 1 << 2,
	LOCATION = 1 << 3,
};

constexpr WebservConfLevel operator|(WebservConfLevel a, WebservConfLevel b) {
	return static_cast<WebservConfLevel>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr WebservConfLevel operator&(WebservConfLevel a, WebservConfLevel b) {
	return static_cast<WebservConfLevel>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

class ConfigParser;

class IWebservModule {
	public:
		virtual ~IWebservModule() = default;
        virtual int isDirectiveValid(const std::string& directive, WebservConfLevel level) = 0;
		virtual void parseDirective(ConfigParser& parser, WebservConfLevel level) = 0;
};

class AWebservParser : virtual public IWebservModule {
    protected:
        const std::unordered_map<std::string, WebservConfLevel> _directiveValidLevels;
    public:
        AWebservParser() = delete;
        AWebservParser(const std::unordered_map<std::string, WebservConfLevel> directiveValidLevels);
        virtual ~AWebservParser() = default;
        int isDirectiveValid(const std::string& directive, WebservConfLevel level) override;
};

class WebservCoreParser : public AWebservParser {
    public:
        WebservCoreParser();
        virtual ~WebservCoreParser() = default;
        void parseDirective(ConfigParser& parser, WebservConfLevel level) override;
};

class WebservCoreModule : public WebservCoreParser {
	public:
        WebservCoreModule() = default;
        virtual ~WebservCoreModule() = default;
};