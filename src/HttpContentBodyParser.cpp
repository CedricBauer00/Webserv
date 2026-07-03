#include "../inc/HttpContentBodyParser.hpp"

HttpContentBodyParser::HttpContentBodyParser() {
	if (_max_size < _contentLength)
		throw PayloadTooLarge();
}

void	HttpContentBodyParser::parse(char* buffer, std::size_t count) {
	_request.append(buffer, count);

	std::size_t remaining = _contentLength - _body.size();
	std::size_t toCopy = std::min(remaining, _request.size());

	_body.append(_request.data(), toCopy);
	_request.erase(0, toCopy);
	if (_body.size() == _contentLength)
		_bodyStopReceived = true;
	return;
}
