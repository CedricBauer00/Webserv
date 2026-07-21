#pragma once

#include <unistd.h>
#include <string>
#include <string_view>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm> 
#include <cctype>
#include "MethodTypes.hpp"
#include "Exceptions.hpp"
#include "constants.h"
#include "Utils.hpp"

class AHttpParser
{
    public:
		struct data {
			std::array<std::string, 3>						startLine;
			std::unordered_map<std::string, std::string>	headers;
			std::string			path;
			std::string			query;
			std::string 		body;
			std::string		    request;
			bool				parseCompleted{false};
			std::string 		hostPort;
			std::string 		hostName;
			method 		        methodType{METHOD_GET};
			unsigned int    	methodMask{methodMap.at("GET").first};
			bool                internalRedirect{false};
			std::size_t			max_size{8192};
		};

    protected:
        std::unique_ptr<data>	_data;

    public:
        AHttpParser();
        AHttpParser(std::string&& request);
        AHttpParser(AHttpParser&& other) noexcept;
        AHttpParser(AHttpParser&& other, std::size_t max_size) noexcept;
        AHttpParser& operator=(AHttpParser&& other) noexcept = default;
        virtual ~AHttpParser();
 
		virtual void	parse(char* buffer, std::size_t count) = 0;
		void		setMethod(const std::string& method);
        std::string trim( const std::string& value );
        void		setRedirectPath(std::string&& redirectPath);
        bool		http1p0Ended();
		void		unsetParseCompleted();

        const std::array<std::string, 3>&				getStartLine() const;
        const std::string&								getQuery() const;
        const std::unordered_map<std::string, std::string>&	getHeaders() const;
        const std::string&								getBody() const;
		const std::string&								getMethodStr() const;
        method										    getMethod() const;
        unsigned int									getMethodMask() const;
        const std::string&								getHostName() const;
        const std::string&								getHostPort() const;
		const std::string&								getPath() const;
        const std::string&                              getHttp() const;
		bool											parseCompleted() const;
		bool											headerHasChunked() const;
		bool											headerHasContlen() const;
		std::string										getRequest();
};
