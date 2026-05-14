#pragma once

#include "HttpParser.hpp"
#include "../inc/HttpException.hpp"
#include "../inc/Exceptions.hpp"
#include "../inc/PageHandler.hpp"
#include "../inc/Response.hpp"
#include "../inc/ConfigParsing.hpp"

class Execution
{
    private:
        std::string _uri;
    public:
        Execution();
        ~Execution();

        void    serverRewrite( std::string uri, std::vector<Server> servers, std::string hostName, std::string hostPort );
        std::string getUri();
        void    execution( std::string request, Response &Res, std::vector<Server> servers );
};
