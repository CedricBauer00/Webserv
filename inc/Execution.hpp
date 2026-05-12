#pragma once

#include "HttpParser.hpp"
#include "../inc/HttpException.hpp"
#include "../inc/Exceptions.hpp"
#include "../inc/PageHandler.hpp"
#include "../inc/Response.hpp"

class Execution
{
    private:
        std::string _uri;
    public:
        Execution();
        ~Execution();

        void    serverRewrite( std::string uri );
        std::string getUri();
        void    execution( std::string request, Response &Res );
};
