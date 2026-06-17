#include "../inc/Response.hpp"

Response::Response() : _response(), _httpVersion(), _statusCode(), _reasonPhrase(), _headers(), _body() {}

Response::Response(Response&& other) noexcept
    : _response(std::move(other._response))
    , _httpVersion(std::move(other._httpVersion))
    , _statusCode(std::move(other._statusCode))
    , _reasonPhrase(std::move(other._reasonPhrase))
    , _headers(std::move(other._headers))
    , _body(std::move(other._body))
{
    std::cout << "Response move constructor called" << std::endl;
}

Response& Response::operator=(Response&& other) noexcept
{
    if (this != &other)
    {
        _response = std::move(other._response);
        _httpVersion = std::move(other._httpVersion);
        _statusCode = std::move(other._statusCode);
        _reasonPhrase = std::move(other._reasonPhrase);
        _headers = std::move(other._headers);
        _body = std::move(other._body);
    }
    return *this;
}

Response::~Response() {}

void    Response::build()
{
    _response = "HTTP/1.0 ";
    _response += _statusCode + " " + _reasonPhrase + "\r\n";

    //set headers
    for (auto& x : _headers)
        _response += x.first + ": " + x.second + "\r\n";

    _response += "Connection: Closed\r\n\r\n";
    _response += _body;

}

void    Response::setBody(std::string&& content)
{
    _body = std::move(content);
}

void    Response::setCodeAndPhrase(const std::string& statusCode,
    const std::string& reasonPhrase )
{
    _statusCode = statusCode;
    _reasonPhrase = reasonPhrase;
}

void    Response::setHeaders(const std::string& key,
    const std::string& content )
{
    _headers[key] = content;
}

const std::string& Response::getResponse() const
{
    return _response;
}

void    Response::clear()
{
    _response.clear();
    _httpVersion.clear();
    _statusCode.clear();
    _reasonPhrase.clear();
    _headers.clear();
    _body.clear();
}