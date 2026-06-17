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
#include "constants.h"

#define MAX_BODY_SIZE 8192

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
		bool				_headStopReceived;
		bool				_bodyStopReceived;
        bool				_foundContlen;
        bool				_foundHost;
        std::size_t			_contentLength;
        bool				_chunked;
        std::string		    _request;
        std::string 		_hostPort;
        std::string 		_hostName;
        whichMethod 		_method;
        unsigned int    	_reqMethodMask;
		std::size_t 		_currentChunkSize = 0;
		bool 				_waitingForChunkData = false;
		bool 				_waitingForLastChunkCRLF = false;

        void	_setMethod();
        void	_checkStartLine();
		void	_decodeRequestTarget(std::string& requestTarget);
		void	_splitRequestTarget(std::string& requestTarget);
		void	_normalizePath();
		// void    _setHttpVersion();

    public:
        HttpParser();
        HttpParser(const std::string& request);
        HttpParser(HttpParser&& other) noexcept;
        HttpParser& operator=(HttpParser&& other) noexcept = default;
        ~HttpParser();
 
        // void        parse();
        void        parseHead(char* buffer, std::size_t count);
        void        parseBody(char* buffer, std::size_t count);
        void        setBody();
        std::string trim( const std::string& value );
        bool        isAllDigits( const std::string& word );
        // void        initIss( std::string request );
        void        checkHostHeader( const std::string& value );
        void        validatePort( std::string portStr );
        void        checkCgiExtension();

        // void    setHttpVersion();
        
        const std::vector<std::string>&					getStartLine() const;
        const std::unordered_map<std::string, std::string>&	getHeaders() const;
        const std::string&								getBody() const;
        whichMethod										getMethod() const;
        unsigned int									getReqMethodMask() const;
        const std::string&								getHostName() const;
        const std::string&								getHostPort() const;
		const std::string&								getPath() const;
        const std::string&                              getHttp() const;
		std::string&									getRequest();
		bool											headStopReceived() const;
		bool											bodyStopReceived() const;
		bool											headerHasContlen() const;
		std::size_t										getContlen() const;
		bool											isHTTP1p0() const;
};

bool    isInRange( int num, int min, int max );

void    HttpParsing( std::string request );

// JSON POST
// falls POST method, check ob eine json (Content-Type: /json), dann ignore erste '{' und letzte '}' character
