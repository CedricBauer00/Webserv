#include "../inc/HttpEOFBodyParser.hpp"

HttpEOFBodyParser::HttpEOFBodyParser() {
	if (_startLine[2] != "HTTP/1.0")
		throw BadRequest();
}

HttpEOFBodyParser::~HttpEOFBodyParser() {
}

void	HttpEOFBodyParser::parse(char* buffer, std::size_t count) {
	if (_max_size < count || (_max_size - count) < _body.size())
		throw PayloadTooLarge();
	_body.append(buffer, count);
}
