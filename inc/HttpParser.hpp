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

#define MAX_BODY_SIZE 1024;

class HttpParser
{
    private:
        std::vector<std::string>            _startLine;
        std::unordered_map<std::string, std::string>  _headers;
        std::string _body;
        std::istringstream  _iss;
        int         _contentLength;
        int         _bodyLength;
        bool        _chunked;


        void        setStartLine( std::string line );
        void        checkStartLine();
        std::string trim( const std::string& value );
        bool        isAllDigits( const std::string& word );
        void        initIss( std::string request );
        
    public:
        HttpParser();
        
        void    setHeaders( std::string request );
        void    setBody();

        std::vector<std::string>            getStartLine();
        std::unordered_map<std::string, std::string>  getHeaders();
        std::string                         getBody();

        ~HttpParser();
};

void    HttpParsing( std::string request );
