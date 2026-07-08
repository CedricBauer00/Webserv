#include "../inc/HttpContentBodyParser.hpp"

HttpContentBodyParser::HttpContentBodyParser() {
	if (_data->max_size < _data->contentLength)
		throw PayloadTooLarge();
}

HttpContentBodyParser::HttpContentBodyParser(
	AHttpParser&& other, std::size_t max_size)
: AHttpParser(std::move(other), max_size) {
	if (_data->max_size < _data->contentLength)
		throw PayloadTooLarge();
}

HttpContentBodyParser::~HttpContentBodyParser() {
}

void	HttpContentBodyParser::parse(char* buffer, std::size_t count) {
	_data->request.append(buffer, count);

	std::size_t remaining = _data->contentLength - _data->body.size();
	std::size_t toCopy = std::min(remaining, _data->request.size());

	_data->body.append(_data->request.data(), toCopy);
	_data->request.erase(0, toCopy);
	if (_data->body.size() == _data->contentLength)
		_data->parseCompleted = true;
}
