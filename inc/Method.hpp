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

		void	_createAutoIndexPage(
			const std::string &path, Response &res,const HttpParser& parser);
		void	_parseCGIResponse(const std::string& cgiRes, Response &htmlRes);
		std::vector<std::string>	_getCGIEnv(const HttpParser& parser);
        bool    _ranCGI(
            const std::string &path, Response &res, const HttpParser& parser);
        void    _runCgi(
            const std::string &path, Response &res, const HttpParser& parser);

    public:
        Method();
        ~Method();
        bool    getMethod(const std::string &path, Response &res,
            HttpParser& parser, const LocIndexConf* locIndexConf);
        bool    postMethod(const std::string &path, Response &res,
            const HttpParser& parser); // status codes 200, 402, 404
        bool    deleteMethod(std::string newPath, Response &res,
			const HttpParser& parser); // status codes 200, 402, 404
};

std::string getTimeStamp();
