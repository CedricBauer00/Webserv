#include "../inc/Response.hpp"

Response::Response() : _response(), _httpVersion(), _statusCode(), _reasonPhrase(), _headers(), _body() {}

Response::~Response() {}

void    Response::build()
{
    std::ostringstream oss;
    // HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n
    oss << "HTTP/1.0 " << _statusCode << " " << _reasonPhrase << "\r\n";
    

    for ( auto x : _headers ) //set headers
    {
        oss << x.first << ": " << x.second << "\r\n";
    }

    oss << "Connection: Closed\r\n\r\n";
    oss << _body;
    _response = oss.str();
}

void    Response::setBody( std::string content )
{
    // <html><body>Hello, World!</body></html>
    _body = content;
}

void    Response::setCodeAndPhrase( std::string statusCode, std::string reasonPhrase )
{
    _statusCode = statusCode;
    _reasonPhrase = reasonPhrase;
}

void    Response::setHeaders( std::string key, std::string content )
{
    _headers[ key ] = content;
}

std::string Response::getResponse() const
{
    return _response;
}