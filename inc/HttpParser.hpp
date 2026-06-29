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

// struct Cookie {
// 	std::string name;
// 	std::string value;
// 	std::string path;
// 	// bool httpOnly;
// };

class HttpParser
{
    private:
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
        method 		        _method;
        unsigned int    	_methodMask;
		std::size_t 		_currentChunkSize = 0;
		bool 				_waitingForChunkData = false;
		bool 				_waitingForLastChunkCRLF = false;
        bool                _internalRedirect = false;

        void	_setMethod();
        void	_checkStartLine();
		void	_decodeRequestTarget(std::string& requestTarget);
		void	_splitRequestTarget(std::string& requestTarget);
		void	_normalizePath();
		// void    _setHttpVersion();
        // std::vector<Cookie> _cookies;
        std::string         _cookieHeader;

    public:
        HttpParser();
        HttpParser(const std::string& request);
        HttpParser(HttpParser&& other) noexcept;
        HttpParser& operator=(HttpParser&& other) noexcept = default;
        ~HttpParser();
 
        void        parseHead(char* buffer, std::size_t count);
        void        parseBody(char* buffer, std::size_t count);
		void		setMethod(const std::string& method);
        std::string trim( const std::string& value );
        void        checkHostHeader( const std::string& value );
        void        validatePort( std::string portStr );
        void		setRedirectPath(std::string&& redirectPath);
        void        parseCookies( const std::string& cookieHeader );
        void        setCookies( std::string value );
        
        const std::vector<std::string>&					getStartLine() const;
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
		bool											headStopReceived() const;
		bool											bodyStopReceived() const;
		bool											headerHasContlen() const;
		std::size_t										getContlen() const;
		bool											isHTTP1p0() const;
        const std::string                               getCookies() const;
};

bool    isInRange( int num, int min, int max );
