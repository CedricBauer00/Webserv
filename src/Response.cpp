#include "../inc/Response.hpp"

Response::Response() : _response() {}

Response::~Response() {}

void        Response::setResponse( std::string Response ) 
{
    _response = Response;
}

void        Response::setErrorPage( std::string ErrorPage )
{
    _response = ErrorPage;
}

std::string Response::getResponse() const
{
    return _response;
}