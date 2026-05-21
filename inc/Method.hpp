# pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Exceptions.hpp"
#include <filesystem>
#include <fstream>
#include "Response.hpp"
#include "MethodTypes.hpp"

class Method
{
    private:
        std::string _path;
    public:
        Method();
        ~Method();
        std::string    modifyPath( std::string uri, whichMethod whichMethod );
        void    getMethod( std::string newPath, Response &res, std::string uri );
        void    postMethod( std::string newPath, Response &res);
        void    deleteMethod( std::string newPath, Response &res);
};

// bool        autoIndexActive();
// void        createAutoIndex( std::string mockUri, Response &res );

std::string getRootPath();
bool        getUploadEnabled();
std::string getUploadPath();