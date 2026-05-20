#pragma once

#include "HttpParser.hpp"
#include "HttpException.hpp"
#include "Exceptions.hpp"
#include "PageHandler.hpp"
#include "Response.hpp"
#include "ConfigParsing.hpp"
#include "Method.hpp"

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
