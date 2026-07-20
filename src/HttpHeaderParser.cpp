#include "../inc/HttpHeaderParser.hpp"

HttpHeaderParser::HttpHeaderParser() {
}

HttpHeaderParser::HttpHeaderParser(std::string&& request)
: AHttpParser(std::move(request)) {
}

HttpHeaderParser::HttpHeaderParser(AHttpParser&& other)
: AHttpParser(std::move(other)) {
}

HttpHeaderParser::~HttpHeaderParser() {
};

void	HttpHeaderParser::parse(char* buffer, std::size_t count) {
	_data->request.append(buffer, count);
    std::string::size_type start = 0;
    std::string::size_type pos;

    while ((pos = _data->request.find("\r\n", start)) != std::string::npos)
    {
        std::string_view line(_data->request.data() + start, pos - start);
        if (line.empty())
        {
            if (_data->startLine.empty())
                throw BadRequest();
            _data->request.erase(0, pos + 2);
			_data->parseCompleted = true;
            return;
        }

        if (_data->startLine.empty())
        {
            std::size_t fieldStart = 0;
            std::size_t fieldEnd = line.find(' ');
            while (fieldEnd != std::string_view::npos) {
                _data->startLine.emplace_back(
					line.substr(fieldStart, fieldEnd - fieldStart));
                fieldStart = fieldEnd + 1;
                fieldEnd = line.find(' ', fieldStart);
            }
            _data->startLine.emplace_back(line.substr(fieldStart));
            _checkStartLine();
            _setMethod();
            _decodeRequestTarget(_data->startLine[1]);
            _splitRequestTarget(_data->startLine[1]);
            _normalizePath();
        }
        else
        {
            std::size_t colon = line.find(':');
            if (colon == std::string_view::npos || colon == 0)
                throw BadRequest();
            if (line[colon - 1] == ' ')
                throw BadRequest();

            std::string key(line.substr(0, colon));
            std::string value(line.substr(colon + 1));

            for (auto& x : key)
                x = tolower(static_cast<unsigned char>(x));

            value = trim(value);
            key = trim(key);
            if (key == "content-length" && _data->foundContlen == false) {
                if (_data->chunked)
                    throw BadRequest();
                if (!isAllDigits(value))
                    throw BadRequest();
                _data->foundContlen = true;
                _data->contentLength = static_cast<std::size_t>(std::stoi(value));
            }
            else if (key == "transfer-encoding") {
				for (auto& x : value)
                	x = tolower(static_cast<unsigned char>(x));
				if (value == "chunked") {
					if (_data->foundContlen)
						throw BadRequest();
					_data->chunked = true;
				}
            }
            else if (key == "host")
                checkHostHeader(value);

            _data->headers[key] = value;
        }

        start = pos + 2;
    }
	_data->request.erase(0, start);
}
