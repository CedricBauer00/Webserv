#pragma once

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm> 
#include <cctype>

#define RED  "\033[31m"
#define ELEC_RED "\033[38;2;255;20;20m"
#define BLUE    "\033[34m"
#define GREEN  "\033[32m"
#define ORANGE  "\033[38;2;255;120;0m"
#define RESET  "\033[0m"

#define MAX_BODY_SIZE 1024

enum Method
{
    METHOD_GET,
    METHOD_POST,
    METHOD_DELETE
};

class HttpParser
{
    private:
        std::vector<std::string>                        _startLine;
        std::unordered_map<std::string, std::string>    _headers;
        std::string _httpVersion;
        std::string _uri;
        std::string _body;
        std::size_t _bodyLength;
        bool        _foundContlen;
        bool        _foundHost;
        std::size_t _contentLength;
        bool        _chunked;
        std::istringstream  _iss;
        int         _hostPort;

        Method  _method;

        void        setStartLine( std::string line );
        void        checkStartLine();
        std::string trim( const std::string& value );
        bool        isAllDigits( const std::string& word );
        void        initIss( std::string request );
        void        checkHostHeader( std::string value );
        
    public:
        HttpParser();
        HttpParser( std::string reqeust );
        
        void    setHeaders( );
        void    setBody();
        void    setMethod();
        void    setUri();
        // void    setHttpVersion();

        std::vector<std::string>            getStartLine();
        std::unordered_map<std::string, std::string>  getHeaders();
        std::string                         getBody() const;
        std::string                         getUri();
        // std::string                         getHttpVersion() const;

        Method  getMethod() const;
        
        ~HttpParser();
};

bool    isInRange( int num, int min, int max );

void    HttpParsing( std::string request );

// JSON POST
// falls POST method, check ob eine json (Content-Type: /json), dann ignore erste '{' und letzte '}' character
