#pragma once

#include "HttpException.hpp"

class BadRequest : public HttpException
{
    public:
        BadRequest() : HttpException( 400, "Bad Request" ) {}
};

class Unauthorized : public HttpException
{
    public:
        Unauthorized() : HttpException( 401, "Unauthorized") {}
};

class Forbidden : public HttpException
{
    public:
        Forbidden() : HttpException( 403, "Forbidden") {}
};

class NotFound : public HttpException
{
    public:
        NotFound() : HttpException( 404, "Not Found" ) {}
};

class MethodNotAllowed : public HttpException
{
    public:
        MethodNotAllowed() : HttpException( 405, "Method Not Allowed") {}
};

class PayloadTooLarge : public HttpException
{
    public:
        PayloadTooLarge() : HttpException( 413, "Payload Too Large") {}
};

class InternalServerError : public HttpException
{
    public:
        InternalServerError() : HttpException( 500, "Internal Server Error") {}
};

class MethodNotImplemented : public HttpException
{
    public:
        MethodNotImplemented() : HttpException( 501, "Not Implemented") {}
};

class BadGateway : public HttpException
{
    public:
        BadGateway() : HttpException( 502, "Bad Gateway") {}
};

class HttpVersionNotSupported : public HttpException
{
    public:
        HttpVersionNotSupported() : HttpException( 505, "Http Version Not Allowed") {}
};

class MovedPermanently : public HttpException
{
    public:
        MovedPermanently(const std::string& location ) : HttpException( 301, "Moved Permanently", location ) {}
};

class Found : public HttpException
{
    public:
        Found(const std::string& location ) : HttpException( 302, "Found", location ) {}
};

class Created : public HttpException
{
    public:
        Created(const std::string& location ) : HttpException( 201, "Created", location ) {}
};