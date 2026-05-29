#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Response.hpp"

class PageHandler
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
        std::string _errorPage;
    public:
        PageHandler( int statusCode, std::string reasonPhrase );
        ~PageHandler();
        
        void        setErrorPage( Response &res );
        void        setRedirectPage( Response &res, std::string uri );
        std::string getErrorPage() const;

};

std::string getFileType( std::string path );