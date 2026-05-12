#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Response.hpp"

class ErrorPageHandler
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
        std::string _errorPage;
    public:
        ErrorPageHandler( int statusCode, std::string reasonPhrase );
        ~ErrorPageHandler();
        
        void        createErrorPage( Response &Res );
        void        setErrorPage();
        std::string getErrorPage() const;

};

std::string getFileType( std::string path );