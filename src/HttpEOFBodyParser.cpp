#include "../inc/HttpEOFBodyParser.hpp"

HttpEOFBodyParser::HttpEOFBodyParser() {
}

HttpEOFBodyParser::HttpEOFBodyParser(AHttpParser&& other, std::size_t max_size)
: AHttpParser(std::move(other), max_size) {
	if (_data->startLine[2] != "HTTP/1.0")
		throw BadRequest();
}

HttpEOFBodyParser::~HttpEOFBodyParser() {
}

void	HttpEOFBodyParser::parse(char* buffer, std::size_t count) {
	if (_data->max_size < count
		|| (_data->max_size - count) < _data->body.size())
		throw PayloadTooLarge();
	_data->body.append(buffer, count);
}
