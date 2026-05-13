#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <unordered_map>
#include <memory>
#include "configParsing.hpp"
#include "modules.hpp"

class ConfigParser {
    public:
		template<typename T>
		using VecOfPtrs = std::vector<std::unique_ptr<T>>;
		struct	WebservHttpConfCtx {
			VecOfPtrs<WebservHttpConf>*	httpConfs = nullptr;
			VecOfPtrs<WebservSrvConf>*	srvConfs = nullptr;
			VecOfPtrs<WebservLocConf>*	locConfs = nullptr;
		};
		WebservHttpConfCtx				httpConfCtx;

		VecOfPtrs<WebservHttpConf>		httpConfs;
		struct  LocConf {
			LocConf*					parent = nullptr;
			VecOfPtrs<WebservLocConf>	locConfs;
			std::vector<LocConf>		locations;
		};
		struct  SrvLocConf {
            VecOfPtrs<WebservSrvConf>	srvConfs;
            LocConf						location;
        };
		std::vector<SrvLocConf>		servers;

        ConfigParser() = delete;
        ConfigParser(char* filename);
        ~ConfigParser();
        void						parseConfig(WebservConfLevel level =
										WebservConfLevel::MAIN);
		std::deque<std::string>&	getTokens();
		const std::string&			getLevelName(WebservConfLevel level) const;

	 private:
		const std::unordered_map<WebservConfLevel, std::string> _levelNames {
			{WebservConfLevel::MAIN, "main"},
			{WebservConfLevel::HTTP, "http"},
			{WebservConfLevel::SERVER, "server"},
			{WebservConfLevel::LOCATION, "location"},
		};
        char*						_configFilename;
        std::deque<std::string>		_tokens;
		VecOfPtrs<IWebservModule>	_modules;

        std::deque<std::string>	tokenize();
		void					parseDirective(WebservConfLevel level);
};
