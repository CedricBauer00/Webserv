#pragma once

#include "HttpException.hpp"

class BadRequest : public HttpException
{
    public:
        BadRequest() : HttpException( 400, "Bad Request" ) {}
        // ~BadRequest();
};

class MethodNotAllowed : public HttpException
{
    public:
        MethodNotAllowed() : HttpException( 405, "Method Not Allowed") {}
};

class HttpVersionNotSupported : public HttpException
{
    public:
        HttpVersionNotSupported() : HttpException( 505, "Http Version Not Allowed") {}
};
