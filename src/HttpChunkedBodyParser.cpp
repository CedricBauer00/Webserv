#include "../inc/HttpChunkedBodyParser.hpp"

HttpChunkedBodyParser::HttpChunkedBodyParser() {
}

HttpChunkedBodyParser::HttpChunkedBodyParser(AHttpParser&& other, std::size_t max_size) noexcept
: AHttpParser(std::move(other), max_size) {
}

HttpChunkedBodyParser::~HttpChunkedBodyParser() {
}

void	HttpChunkedBodyParser::parse(char* buffer, std::size_t count) {
	_data->request.append(buffer, count);

	while (true) {
		if (!_waitingForChunkData) {
			std::string::size_type sizeEnd = _data->request.find("\r\n");

			if (sizeEnd == std::string::npos)
				return;
			for (size_t i = 0; i < sizeEnd; ++i) {
				if (!std::isxdigit(
						static_cast<unsigned char>(_data->request[i])))
					throw BadRequest();
			}

			try {
				_currentChunkSize =
					std::stoul(_data->request.substr(0, sizeEnd), nullptr, 16);
			}
			catch (...) {
				throw BadRequest();
			}

			_data->request.erase(0, sizeEnd + 2);
			if (_currentChunkSize == 0) {
				_waitingForLastChunkCRLF = true;
				_waitingForChunkData = true;
				continue;
			}
			if(_data->max_size < _currentChunkSize
				|| (_data->max_size - _currentChunkSize) < _data->body.size())
				throw PayloadTooLarge();
			_waitingForChunkData = true;
		}

		if (_waitingForLastChunkCRLF) {
			if (_data->request.size() < 2)
				return;
			if (_data->request[0] != '\r' || _data->request[1] != '\n')
				throw BadRequest();
			_data->request.erase(0, 2);
			_data->parseCompleted = true;
			return;
		}

		if (_data->request.size() < _currentChunkSize + 2)
			return;
		if (_data->request[_currentChunkSize] != '\r'
			|| _data->request[_currentChunkSize + 1] != '\n')
			throw BadRequest();
		_data->body.append(_data->request.data(), _currentChunkSize);
		_data->request.erase(0, _currentChunkSize + 2);
		_currentChunkSize = 0;
		_waitingForChunkData = false;
	}
}