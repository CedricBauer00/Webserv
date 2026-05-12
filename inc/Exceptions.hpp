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

class PayloadTooLarge : public HttpException
{
    public:
        PayloadTooLarge() : HttpException( 413, "Payload Too Large") {}
};

class Unautthorized : public HttpException
{
    public:
        Unautthorized() : HttpException( 401, "Unauthorized") {}
};

class Forbidden : public HttpException
{
    public:
        Forbidden() : HttpException( 403, "Forbidden") {}
};

// redirects
class MovedPermanently : public HttpException
{
    public:
        MovedPermanently( std::string& location ) : HttpException( 301, "Moved Permanently", location ) {}
};

class Found : public HttpException
{
    public:
        Found( std::string& location ) : HttpException( 302, "Found", location ) {}
};