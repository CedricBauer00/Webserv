#pragma once

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm> 
#include <cctype>

// #include "Execution.hpp"
#include "MethodTypes.hpp"
// #include "HttpException.hpp"
#include "Exceptions.hpp"

#define RED  "\033[31m"
#define ELEC_RED "\033[38;2;255;20;20m"
#define BLUE    "\033[34m"
#define GREEN  "\033[32m"
#define ORANGE  "\033[38;2;255;120;0m"
#define RESET  "\033[0m"

#define MAX_BODY_SIZE 1024

class HttpParser
{
    private:
        inline static const std::unordered_map<std::string, unsigned int> m = {
        {"GET", 1u<<0}, {"POST", 1u<<1}, {"PUT", 1u<<2},
        {"DELETE", 1u<<3}, {"HEAD", 1u<<4}, {"OPTIONS", 1u<<5}};
        std::vector<std::string>						_startLine;
        std::unordered_map<std::string, std::string>	_headers;
        std::string			_httpVersion;
        std::string			_path;
		std::string			_query;
        std::string 		_body;
        std::size_t			_bodyLength;
        bool				_foundContlen;
        bool				_foundHost;
        std::size_t			_contentLength;
        bool				_chunked;
        std::istringstream	_request;
        std::string 		_hostPort;
        std::string 		_hostName;
        whichMethod 		_method;
        unsigned int    	_reqMethodMask;

        void	_setMethod();
        void	_setStartLine(std::istringstream line);
        void	_checkStartLine();
		void	_decodeRequestTarget(std::string& requestTarget);
		void	_splitRequestTarget(std::string& requestTarget);
		void	_normalizePath();
		void	_getKeyAndValue(
			const std::string& line, std::string& key, std::string& value);
		// void    _setHttpVersion();

    public:
        HttpParser() = delete;
        HttpParser(const std::string& request);
        HttpParser(HttpParser&& other) noexcept;
        HttpParser& operator=(HttpParser&& other) noexcept = default;
        ~HttpParser();
 
        void        parse();
        void        setBody();
        std::string trim( const std::string& value );
        bool        isAllDigits( const std::string& word );
        void        initIss( std::string request );
        void        checkHostHeader( std::string value );
        void        validatePort( std::string portStr );
        void        checkCgiExtension();

        // void    setHttpVersion();
        
        std::vector<std::string>						getStartLine();
        std::unordered_map<std::string, std::string>	getHeaders();
        const std::string&								getBody() const;
        whichMethod										getMethod() const;
        unsigned int									getReqMethod() const;
        const std::string&								getHostName() const;
        const std::string&								getHostPort() const;
		const std::string&								getPath() const;
};

bool    isInRange( int num, int min, int max );

void    HttpParsing( std::string request );

// JSON POST
// falls POST method, check ob eine json (Content-Type: /json), dann ignore erste '{' und letzte '}' character
