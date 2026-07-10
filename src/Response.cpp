#include "../inc/Response.hpp"
#include "HttpException.hpp"
#include "statusCodes.hpp"

Response::Response() {
}

Response::~Response() {
}

void    Response::build() {
    _response = "HTTP/1.0 " + _statusCode + " " + _reasonPhrase + "\r\n";

    //set headers
    for (auto& x : _headers)
        _response += x.first + ": " + x.second + "\r\n";

    _response += "Connection: Closed\r\n\r\n";
}

void	Response::build(HttpException&& e) {
	build(std::move(e).getHeaders(), std::to_string(e.getStatusCode()),
		std::move(e).getReasonPhrase());
}

void	Response::build(std::string&& content,
	std::unordered_map<std::string, std::string>&& headers,
	std::string statusCode,
	std::string reasonPhrase) {
	setBody(std::move(content));
	build(std::move(headers), std::move(statusCode), std::move(reasonPhrase));
}

void	Response::build(std::unordered_map<std::string, std::string>&& headers,
	std::string statusCode,
	std::string reasonPhrase) {
	_headers = std::move(headers);
	build(std::move(statusCode), std::move(reasonPhrase));
}

void	Response::build(std::string statusCode,
	std::string reasonPhrase) {
	setCodeAndPhrase(std::move(statusCode), std::move(reasonPhrase));
	build();
}

void    Response::setBody(std::string&& content) {
    _body = std::move(content);
}

void    Response::setCodeAndPhrase(std::string statusCode,
    std::string reasonPhrase ) {
    if (!_internalRedirect) {
        _statusCode = std::move(statusCode);
        _reasonPhrase = std::move(reasonPhrase);
    }
}

void	Response::setRedirect(unsigned long statusCode) {
	setCodeAndPhrase(std::to_string(statusCode),
		statusCodeToReasonPhrase.at(statusCode));
	_internalRedirect = true;
}

void    Response::setHeaders(const std::string& key,
    const std::string& content ) {
    _headers[key] = content;
}

void	Response::mvBodyToText() {
	_response = std::move(_body);
	_body.clear();  //handles the case where an application implements std::move() as copy and leaves _body non-empty.
}

const std::string& Response::getText() const {
    return _response;
}

bool	Response::wasRedirected() const {
	return _internalRedirect;
}

std::size_t	Response::bodySize() const {
	return _body.size();
}

void    Response::clear() {
    _response.clear();
    _httpVersion.clear();
    _statusCode.clear();
    _reasonPhrase.clear();
    _headers.clear();
    _body.clear();
}