#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class ErrorPageHandler
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
    public:
        ErrorPageHandler( int statusCode, std::string reasonPhrase );
        ~ErrorPageHandler();
        
        void Handler();

};