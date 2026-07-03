#include "../inc/HttpEOFBodyParser.hpp"

void	HttpEOFBodyParser::parse(char* buffer, std::size_t count) {
	if (_max_size < count || (_max_size - count) < _body.size())
		throw PayloadTooLarge();
	_body.append(buffer, count);
	return;
}
