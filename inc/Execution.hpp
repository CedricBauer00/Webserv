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
        HttpParser& _parser;

    public:
        Execution() = delete;
        Execution(HttpParser& parser);
        ~Execution();

        // void    serverRewrite(std::string uri,
        //     const std::vector<const IWebservModule::Srv*>& servers,
        //     std::string hostName, std::string hostPort);
        // std::string getUri();
        static const IWebservModule::LocNode*	selectLocation(const std::string& uri,
            const IWebservModule::LocNode& root);
        void    execution(Response &Res,
            std::function<const IWebservModule::Srv*(const std::string&)> selectServer);
};
