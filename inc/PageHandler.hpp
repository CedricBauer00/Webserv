#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "Response.hpp"

class PageHandler
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
        std::map<int, std::string> errorPages;
    public:
        PageHandler( int statusCode, std::string reasonPhrase );
        ~PageHandler();
        
        void        setErrorPage( Response &res );
        void        setRedirectPage( Response &res, std::string uri );
        void        initErrorPages( const std::map<int, std::string>& configErroPages );


};

std::string getFileType( std::string path );