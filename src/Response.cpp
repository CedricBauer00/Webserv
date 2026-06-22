#include "../inc/Response.hpp"
#include "HttpException.hpp"
#include "modules.hpp"
#include "statusCodes.hpp"

Response::Response() : _response(), _httpVersion(), _statusCode(), _reasonPhrase(), _headers(), _body() {}

Response::Response(Response&& other) noexcept
    : _response(std::move(other._response))
    , _httpVersion(std::move(other._httpVersion))
    , _statusCode(std::move(other._statusCode))
    , _reasonPhrase(std::move(other._reasonPhrase))
    , _headers(std::move(other._headers))
    , _body(std::move(other._body))
	, _internalRedirect(other._internalRedirect)
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
		_internalRedirect = other._internalRedirect;
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

void	Response::build(HttpException&& e) {
	build(std::move(e).getHeaders(), std::to_string(e.getStatusCode()),
		std::move(e).getReasonPhrase());
}

void	Response::build(std::string&& content,
	std::unordered_map<std::string, std::string>&& headers,
	std::string statusCode,
	std::string reasonPhrase)
{
	setBody(std::move(content));
	build(std::move(headers), std::move(statusCode), std::move(reasonPhrase));
}

void	Response::build(std::unordered_map<std::string, std::string>&& headers,
	std::string statusCode,
	std::string reasonPhrase)
{
	_headers = std::move(headers);
	build(std::move(statusCode), std::move(reasonPhrase));
}

void	Response::build(std::string statusCode,
	std::string reasonPhrase)
{
	setCodeAndPhrase(std::move(statusCode), std::move(reasonPhrase));
	build();
}

void    Response::setBody(std::string&& content)
{
    _body = std::move(content);
}

void    Response::setCodeAndPhrase(std::string statusCode,
    std::string reasonPhrase )
{
    _statusCode = std::move(statusCode);
    _reasonPhrase = std::move(reasonPhrase);
}

void	Response::setCodeAndPhrase(const ErrorPage& e) {
	setCodeAndPhrase(std::to_string(e.resCode),
		statusCodeToReasonPhrase.at(e.resCode));
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