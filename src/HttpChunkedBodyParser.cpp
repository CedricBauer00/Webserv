#include "../inc/HttpChunkedBodyParser.hpp"

void	HttpChunkedBodyParser::parse(char* buffer, std::size_t count) {
	_request.append(buffer, count);

	while (true) {
		if (!_waitingForChunkData) {
			std::string::size_type sizeEnd = _request.find("\r\n");

			if (sizeEnd == std::string::npos)
				return;
			for (size_t i = 0; i < sizeEnd; ++i) {
				if (!std::isxdigit(
						static_cast<unsigned char>(_request[i])))
					throw BadRequest();
			}

			try {
				_currentChunkSize =
					std::stoul(_request.substr(0, sizeEnd), nullptr, 16);
			}
			catch (...) {
				throw BadRequest();
			}

			_request.erase(0, sizeEnd + 2);
			if (_currentChunkSize == 0) {
				_waitingForLastChunkCRLF = true;
				continue;
			}
			if(_max_size < _body.size() + _currentChunkSize)
				throw PayloadTooLarge();
			_waitingForChunkData = true;
		}

		if (_waitingForLastChunkCRLF) {
			if (_request.size() < 2)
				return;
			if (_request[0] != '\r' || _request[1] != '\n')
				throw BadRequest();
			_request.erase(0, 2);
			_bodyStopReceived = true;
			_waitingForLastChunkCRLF = false;
			return;
		}

		if (_request.size() < _currentChunkSize + 2)
			return;
		if (_request[_currentChunkSize] != '\r'
			|| _request[_currentChunkSize + 1] != '\n')
			throw BadRequest();
		_body.append(_request.data(), _currentChunkSize);
		_request.erase(0, _currentChunkSize + 2);
		_currentChunkSize = 0;
		_waitingForChunkData = false;
	}
}