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

class Method
{
    private:
        std::string _path;
        std::string _query;
        std::string _postedFile;
        std::string _fileContent;
        bool        _isCgiFile;

        void    _setResponse(Response &res,
            const std::string& statusCode,
            const std::string& reasonPhrase,
            const std::string& path);

    public:
        Method();
        ~Method();
        void    getMethod( std::string newPath, Response &res, const LocNode& location );
        void    postMethod( std::string newPath, Response &res, std::string contentBody, const LocNode& location ); // status codes 200, 402, 404
        void    deleteMethod( std::string newPath, Response &res, const LocNode& location ); // status codes 200, 402, 404
        void    runCgi( std::string &content, bool isPost );
        std::string getCgiPath();
        std::string getScript();
        void    checkCgiExtension();
};

// bool        autoIndexActive();
// void        createAutoIndex( std::string mockUri, Response &res );

std::string getRootPath();
bool        getUploadEnabled();
std::string getUploadPath();
bool        getAllowDeleteDir();
bool        getAllowedToOverwrite();
std::string getTimeStamp();
bool        getIsCgiLocation();
bool        autoIndexActive(); // still to implement: return bool for autoindex
