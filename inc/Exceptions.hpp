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

// response codes function

// void Response::setStatus(int code)
// {
//     static const std::map<int, std::string> reasons = {
//         // {200, "OK"},
//         {201, "Created"},
//         {204, "No Content"},
//         {301, "Moved Permanently"},
//         {308, "Permanent Redirect"},
//         {400, "Bad Request"},
//         {401, "Unauthorized"},
//         {402, "Payment Required"},
//         {403, "Forbidden"},
//         {404, "Not Found"},
//         {405, "Method Not Allowed"},
//         {406, "Not Acceptable"},
//         {407, "Proxy Authentication Required"},
//         {408, "Request Timeout"},
//         {409, "Conflict"},
//         {410, "Gone"},
//         {411, "Length Required"},
//         {412, "Precondition Failed"},
//         {413, "Payload Too Large"},
//         {414, "URI Too Long"},
//         {415, "Unsupported Media Type"},
//         {416, "Range Not Satisfiable"},
//         {417, "Expectation Failed"},
//         {418, "I'm a teapot"},
//         {421, "Misdirected Request"},
//         {422, "Unprocessable Entity"},
//         {423, "Locked"},
//         {424, "Failed Dependency"},
//         {425, "Too Early"},
//         {426, "Upgrade Required"},
//         {428, "Precondition Required"},
//         {429, "Too Many Requests"},
//         {431, "Request Header Fields Too Large"},
//         {451, "Unavailable For Legal Reasons"},
//         {500, "Internal Server Error"},
//         {501, "Not Implemented"},
//         {502, "Bad Gateway"},
//         {503, "Service Unavailable"},
//         {504, "Gateway Timeout"},
//         {505, "HTTP Version Not Supported"},
//         {506, "Variant Also Negotiates"},
//         {507, "Insufficient Storage"},
//         {508, "Loop Detected"},
//         {510, "Not Extended"},
//         {511, "Network Authentication Required"}
//     };
//     _statusCode = code;
//     if (reasons.count(code))
//         _reasonPhrase = reasons.at(code);
//     else
//         _reasonPhrase = "Unknown";
// }