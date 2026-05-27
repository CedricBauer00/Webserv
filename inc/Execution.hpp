#pragma once

#include <vector>
#include "HttpParser.hpp"
#include "HttpException.hpp"
#include "Exceptions.hpp"
#include "PageHandler.hpp"
#include "Response.hpp"
#include "Configparsing/ConfigParser.hpp"
// #include "Method.hpp"

class Execution
{
    private:
        std::string _uri;

    public:
        Execution();
        ~Execution();

        // void    serverRewrite(std::string uri,
        //     const std::vector<const IWebservModule::SrvNode*>& servers,
        //     std::string hostName, std::string hostPort);
        // std::string getUri();
        void    execution(std::string request,
            Response &Res,
            const std::vector<const IWebservModule::SrvNode*>&  servers );
};
