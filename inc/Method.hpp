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
#include <sys/wait.h>

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
        std::string    modifyPath( std::string uri, whichMethod whichMethod );
        void    getMethod( std::string newPath, Response &res );
        void    postMethod( std::string newPath, Response &res, std::string contentBody ); // status codes 200, 402, 404
        void    deleteMethod( std::string newPath, Response &res ); // status codes 200, 402, 404
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
