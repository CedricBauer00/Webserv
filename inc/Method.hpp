# pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Exceptions.hpp"
#include <filesystem>
#include <fstream>
#include "Response.hpp"
#include "MethodTypes.hpp"
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include "../inc/Configparsing/WebservCoreModule.hpp"
#include "../inc/Configparsing/WebservIndexModule.hpp"
#include "HttpParser.hpp"

class Method
{
    private:
        std::string _fileContent;

        void    _setResponse(Response &res,
            const std::string& statusCode,
            const std::string& reasonPhrase,
            const std::string& path);
		void	_createAutoIndexPage(
			const std::string &path, Response &res,const HttpParser& parser);

    public:
        Method();
        ~Method();
        void    getMethod(const std::string &path, Response &res,
            const HttpParser& parser, const LocIndexConf* locIndexConf);
        void    postMethod(const std::string &path, Response &res,
            const HttpParser& parser); // status codes 200, 402, 404

        void    deleteMethod(std::string newPath, Response &res); // status codes 200, 402, 404
        void    runCgi(const std::string &path, Response &res, const HttpParser& parser);
};

std::string getTimeStamp();
