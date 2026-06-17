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

class HttpParser;

class Method
{
    private:
        std::string _path;
        std::string _postedFile;
        std::string _fileContent;

        void    _setResponse(Response &res,
            const std::string& statusCode,
            const std::string& reasonPhrase,
            const std::string& path);

    public:
        Method();
        ~Method();
        void    getMethod( std::string newPath, Response &res, const LocNode& location );
        // void    postMethod( std::string newPath, Response &res, std::string contentBody, const LocNode& location ); // status codes 200, 402, 404
        void    postMethod( std::string path, Response &res, std::string contentBody, const LocNode& location, std::unordered_map<std::string, std::string>	headers ); // status codes 200, 402, 404

        void    deleteMethod(std::string newPath, Response &res); // status codes 200, 402, 404
        void    runCgi(const std::string &path, Response &res, const HttpParser& parser);
        void    checkCgiExtension();
};

std::string getTimeStamp();
