#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "Utils.hpp"


class Response
{
    private:   
        std::string _response;
        std::string _httpVersion;
        std::string _statusCode;
        std::string _reasonPhrase;
        std::unordered_map<std::string, std::string>    _headers;
        std::string _body;

    public:
        Response();
        ~Response();

        void        build();
        void        setBody( std::string content );
        void        setCodeAndPhrase( std::string statusCode, std::string reasonPhrase );
        void        setHeaders( std::string key, std::string content );
        std::string getResponse() const;

};

// std::string setStatus(int code);
