#include "../inc/AHttpParser.hpp"
#include "../inc/Exceptions.hpp"

AHttpParser::AHttpParser() : _data(std::make_unique<data>()) {
}

AHttpParser::AHttpParser(std::string&& request) 
: _data(std::make_unique<data>()) {
    _data->request = std::move(request);
}

AHttpParser::AHttpParser(AHttpParser&& other) noexcept
: _data(std::move(other._data)) {
	// std::cout << "AHttpParser move constructor called" << std::endl;
}

AHttpParser::AHttpParser(AHttpParser&& other, std::size_t max_size) noexcept
: AHttpParser(std::move(other)) {
    _data->max_size = max_size; 
}

AHttpParser::~AHttpParser() {
}

void    AHttpParser::setMethod(const std::string& method) {
    auto it = methodMap.find(method);
    if (it == methodMap.end())
		throw MethodNotImplemented();
	_data->methodMask = it->second.first;
	_data->methodType = it->second.second;    
}

std::string AHttpParser::trim( const std::string& value ) {
    std::string::size_type start = 0;
    while ( start < value.size() && std::isspace( static_cast<unsigned char>( value[ start ] ) ) )
        ++start;

    std::string::size_type end = value.size();
    while ( end > start && std::isspace( static_cast<unsigned char>( value[ end - 1 ] ) ) )
        --end;
    return ( value.substr( start, end - start ) );
}

void	AHttpParser::setRedirectPath(std::string&& redirectPath) {
	_data->path = std::move(redirectPath);
	_data->internalRedirect = true;
}

const std::array<std::string, 3>&    AHttpParser::getStartLine() const {
	return _data->startLine;
}

const std::string&	AHttpParser::getQuery() const {
	return _data->query;
}

const std::unordered_map<std::string, std::string>&    AHttpParser::getHeaders() const {
	return _data->headers;
}

const std::string&    AHttpParser::getBody() const {
    return _data->body;
}

const std::string&	AHttpParser::getMethodStr() const {
	return _data->startLine[0];
}

method  AHttpParser::getMethod() const {
    return _data->methodType;
}

unsigned int    AHttpParser::getMethodMask() const {
    return _data->methodMask;
}

const std::string&     AHttpParser::getHostName() const {
    return _data->hostName;
}

const std::string&     AHttpParser::getHostPort() const {
    return _data->hostPort;
}

const std::string&     AHttpParser::getPath() const {
    return _data->path;
}

const std::string&	AHttpParser::getHttp() const {
	return  _data->startLine[2];
}

bool	AHttpParser::parseCompleted() const {
	return _data->parseCompleted;
}

void	AHttpParser::unsetParseCompleted() {
	_data->parseCompleted = false;
}

bool	AHttpParser::headerHasChunked() const {
	if (_data->headers.find("transfer-encoding") == _data->headers.end())
		return false;
	return _data->headers.at("transfer-encoding") == "chunked";
}

bool	AHttpParser::headerHasContlen() const {
	return _data->headers.find("content-length") != _data->headers.end();
}

std::string AHttpParser::getRequest() {
	return std::move(_data->request);
};

bool	AHttpParser::http1p0Ended() {
	if (_data->startLine[2] == "HTTP/1.0") {
		_data->parseCompleted = true;
		return true;
	}
	return false;
}
