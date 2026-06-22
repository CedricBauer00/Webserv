#include "../inc/HttpException.hpp"

HttpException::HttpException(int statusCode, std::string&& reasonPhrase)
	: _statusCode(statusCode),
	_reasonPhrase(std::move(reasonPhrase))
{}

HttpException::HttpException(int statusCode, std::string&& reasonPhrase,
	std::unordered_map<std::string, std::string>&& headers)
    : _statusCode(statusCode),
	_reasonPhrase(std::move(reasonPhrase)),
	_headers(headers)
{}

int HttpException::getStatusCode() const {
	return _statusCode;
}

const std::string& HttpException::getReasonPhrase() const {
	return _reasonPhrase;
}

const std::unordered_map<std::string, std::string>& HttpException::getHeaders() const {
	return _headers;
}

HttpException::~HttpException() {}