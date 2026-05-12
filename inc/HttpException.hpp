#pragma once

#include <string>
#include <iostream>
// #include <map>

class HttpException : public std::exception
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
        std::string _location;
    public:
        HttpException( int statusCode, std::string reasonPhrase );
        HttpException( int statusCode, std::string reasonPhrase, std::string location );
        ~HttpException();
        
        int         getStatusCode() const;
        std::string getReasonPhrase() const;
        std::string getLocation() const;
};


