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
        //     const std::vector<const IWebservModule::Srv*>& servers,
        //     std::string hostName, std::string hostPort);
        // std::string getUri();
        static const IWebservModule::Srv*	selectServer(const std::string& hostname,
			const std::vector<const IWebservModule::Srv*>& servers);
        static const IWebservModule::LocNode*	selectLocation(const std::string& uri,
            const IWebservModule::LocNode& root);
        void    execution(const std::string& request,
            Response &Res,
            const std::vector<const IWebservModule::Srv*>&  servers );
};
