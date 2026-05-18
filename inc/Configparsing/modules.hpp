#pragma once

#include <unordered_map>
#include "configParsing.hpp"

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

class ConfigParser;

class IWebservModule {
	public:
		virtual ~IWebservModule() = default;
        virtual int isDirectiveValid(const std::string& directive,
			WebservConfLevel level) = 0;
		virtual void parseDirective(ConfigParser& parser,
			WebservConfLevel level) = 0;
};

class AWebservParser : virtual public IWebservModule {
    protected:
        const int												_ctxIndex;
        const std::unordered_map<std::string, WebservConfLevel>	_directiveValLevelMap;

        template<typename T>
		void	_insertConf(VecOfPtrs<T>* confs, std::unique_ptr<T> conf);
    public:
        AWebservParser() = delete;
        AWebservParser(
			int& ctxIndex,
			const std::unordered_map<std::string, WebservConfLevel> directiveValLevelMap);
        virtual ~AWebservParser() = default;
        int isDirectiveValid(const std::string& directive,
			WebservConfLevel level) override;
};

class WebservCoreParser : public AWebservParser {
    public:
        WebservCoreParser() = delete;
		WebservCoreParser(int& ctxIndex);
        virtual ~WebservCoreParser() = default;
        void parseDirective(ConfigParser& parser,
			WebservConfLevel level) override;
};

class WebservCoreModule : public WebservCoreParser {
	public:
        WebservCoreModule() = delete;
		WebservCoreModule(int& ctxIndex);
        virtual ~WebservCoreModule() = default;
};