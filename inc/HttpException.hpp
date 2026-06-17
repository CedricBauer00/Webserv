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
        HttpException(int statusCode, const std::string& reasonPhrase);
        HttpException(int statusCode,
			const std::string& reasonPhrase, const std::string& location);
        ~HttpException();
        
        int					getStatusCode() const;
        const std::string&	getReasonPhrase() const;
        const std::string&	getLocation() const;
};


