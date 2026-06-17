#include "../inc/HttpException.hpp"

HttpException::HttpException(int statusCode, const std::string& reasonPhrase)
: _statusCode( statusCode ), _reasonPhrase( reasonPhrase ) {}

HttpException::HttpException(int statusCode,
    const std::string& reasonPhrase, const std::string& location)
    : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ), _location( location ) {}

int HttpException::getStatusCode() const { return _statusCode; }

const std::string& HttpException::getReasonPhrase() const { return _reasonPhrase; }

const std::string& HttpException::getLocation() const { return _location; }

HttpException::~HttpException() {}