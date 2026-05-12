#include "../inc/HttpException.hpp"

HttpException::HttpException( int statusCode, std::string reasonPhrase ) : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ) {}

HttpException::HttpException( int statusCode, std::string reasonPhrase, std::string location ) : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ), _location( location ) {}

int HttpException::getStatusCode() const { return _statusCode; }

std::string HttpException::getReasonPhrase() const { return _reasonPhrase; }

std::string HttpException::getLocation() const { return _location; }

HttpException::~HttpException() {}