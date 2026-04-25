#include "../inc/HttpException.hpp"

HttpException::HttpException( int statusCode, std::string reasonPhrase ) : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ) {}

int         HttpException::getStatusCode() const { return _statusCode; }

std::string HttpException::getReasonPhrase() const { return _reasonPhrase; }

HttpException::~HttpException() {}