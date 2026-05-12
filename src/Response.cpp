#include "../inc/Response.hpp"

Response::Response() : _response(), _httpVersion(), _statusCode(), _reasonPhrase() {}

Response::~Response() {}

void    Response::build()
{
    std::stringstream ss;
    // HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n
    ss << "HTTP/1.0 " << _statusCode << " " << _reasonPhrase << "\r\n";
    

    // for ( auto x : _headers ) set headers

    ss << "Connection: close\r\n\r\n";
    ss << _body;
    ss >> _response;
}

void    Response::setBody( std::string content )
{
    // <html><body>Hello, World!</body></html>
    _body = content;
}

std::string Response::getResponse() const
{
    return _response;
}