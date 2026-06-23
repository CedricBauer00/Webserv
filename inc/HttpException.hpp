#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

class HttpException : public std::exception
{
    private:
        unsigned long	_statusCode;
        std::string		_reasonPhrase;
        std::unordered_map<std::string, std::string> _headers;

    public:
        HttpException(unsigned long statusCode, std::string reasonPhrase);
        HttpException(unsigned long statusCode, std::string reasonPhrase,
            std::unordered_map<std::string, std::string>&& headers);
        ~HttpException();
        
        unsigned long		getStatusCode() const;
        const std::string&	getReasonPhrase() const &;
        const std::unordered_map<std::string, std::string>&	getHeaders() const &;
        std::string			getReasonPhrase() &&;
		std::unordered_map<std::string, std::string>	getHeaders() &&;
};


