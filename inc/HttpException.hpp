#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

class HttpException : public std::exception
{
    private:
        int         _statusCode;
        std::string _reasonPhrase;
        std::unordered_map<std::string, std::string> _headers;

    public:
        HttpException(int statusCode, std::string reasonPhrase);
        HttpException(int statusCode, std::string reasonPhrase,
            std::unordered_map<std::string, std::string>&& headers);
        ~HttpException();
        
        int					getStatusCode() const;
        const std::string&	getReasonPhrase() const &;
        const std::unordered_map<std::string, std::string>&	getHeaders() const &;
        std::string			getReasonPhrase() &&;
		std::unordered_map<std::string, std::string>	getHeaders() &&;
};


