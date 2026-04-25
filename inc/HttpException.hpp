#pragma once

#include <string>
#include <iostream>
// #include <map>

class HttpException : public std::exception
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
    public:
        HttpException( int statusCode, std::string reasonPhrase );
        ~HttpException();
        
        int         getStatusCode() const;
        std::string getReasonPhrase() const;
};