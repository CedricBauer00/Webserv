#pragma once

#include "HttpException.hpp"
#include "statusCodes.hpp"

class PermanentRedirect : public HttpException
{
    public:
        PermanentRedirect(std::unordered_map<std::string, std::string>&& headers)
		: HttpException(301,
			statusCodeToReasonPhrase.at(301),
			std::move(headers)) {}
};

class TemporaryRedirect : public HttpException
{
    public:
        TemporaryRedirect(std::unordered_map<std::string, std::string>&& headers)
		: HttpException(302,
			statusCodeToReasonPhrase.at(302),
			std::move(headers)) {}
};

class BadRequest : public HttpException
{
    public:
        BadRequest() : HttpException(400, statusCodeToReasonPhrase.at(400)) {}
};

class Unauthorized : public HttpException
{
    public:
        Unauthorized() : HttpException(401, statusCodeToReasonPhrase.at(401)) {}
};

class Forbidden : public HttpException
{
    public:
        Forbidden() : HttpException(403, statusCodeToReasonPhrase.at(403)) {}
};

class NotFound : public HttpException
{
    public:
        NotFound() : HttpException(404, statusCodeToReasonPhrase.at(404)) {}
};

class MethodNotAllowed : public HttpException
{
    public:
        MethodNotAllowed(std::unordered_map<std::string, std::string>&& headers)
		: HttpException(405,
			statusCodeToReasonPhrase.at(405),
			std::move(headers)) {}
};

class PayloadTooLarge : public HttpException
{
    public:
        PayloadTooLarge() : HttpException(413, statusCodeToReasonPhrase.at(413)) {}
};

class InternalServerError : public HttpException
{
    public:
        InternalServerError() : HttpException(500, statusCodeToReasonPhrase.at(500)) {}
};

class MethodNotImplemented : public HttpException
{
    public:
        MethodNotImplemented() : HttpException(501, statusCodeToReasonPhrase.at(501)) {}
};

class BadGateway : public HttpException
{
    public:
        BadGateway() : HttpException(502, statusCodeToReasonPhrase.at(502)) {}
};

class HttpVersionNotSupported : public HttpException
{
    public:
        HttpVersionNotSupported() : HttpException(505, statusCodeToReasonPhrase.at(505)) {}
};
