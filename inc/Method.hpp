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

class Method
{
    private:
        std::string _path;
        std::string _query;
        std::string _postedFile;
        bool        _isCgiFile;

    public:
        Method();
        ~Method();
        std::string    joinRootAndPath(std::string uri, whichMethod whichMethod, const IWebservModule::LocNode& location);
        void    getMethod( std::string newPath, Response &res, const IWebservModule::LocNode& location );
        void    postMethod( std::string newPath, Response &res, std::string contentBody, const IWebservModule::LocNode& location ); // status codes 200, 402, 404
        void    deleteMethod( std::string newPath, Response &res, const IWebservModule::LocNode& location ); // status codes 200, 402, 404
        void    runCgi( std::string &content, bool isPost );
        std::string getCgiPath();
        std::string getScript();
        void    checkCgiExtension();
        std::string    normalizePath(std::string uri);

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
