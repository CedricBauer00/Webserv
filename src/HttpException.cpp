#include "../inc/HttpException.hpp"

HttpException::HttpException(unsigned long statusCode, std::string reasonPhrase)
	: _statusCode(statusCode),
	_reasonPhrase(std::move(reasonPhrase))
{}

HttpException::HttpException(unsigned long statusCode, std::string reasonPhrase,
	std::unordered_map<std::string, std::string>&& headers)
    : _statusCode(statusCode),
	_reasonPhrase(std::move(reasonPhrase)),
	_headers(std::move(headers))
{}

unsigned long	HttpException::getStatusCode() const {
	return _statusCode;
}

const std::string& HttpException::getReasonPhrase() const & {
	return _reasonPhrase;
}

const std::unordered_map<std::string, std::string>& HttpException::getHeaders() const & {
	return _headers;
}

std::string	HttpException::getReasonPhrase() && {
	return std::move(_reasonPhrase);
}

std::unordered_map<std::string, std::string>	HttpException::getHeaders() && {
	return std::move(_headers);
}

HttpException::~HttpException() {}