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

void	HttpHeaderParser::_checkProtocol() {
    if (_data->startLine[2].compare(0, 4, "HTTP") != 0) {
        std::cerr << RED << "Not an HTTP protocol" << RESET << std::endl;
        throw BadRequest();
    }
    if (_data->startLine[2].compare(4, 4, "/1.0") != 0
	&& _data->startLine[2].compare(4, 4, "/1.1") != 0) {
        std::cerr << RED << "HTTP Version not supported" << RESET << std::endl;
        throw HttpVersionNotSupported();
    }
}

void	HttpHeaderParser::_setMethod() {
    setMethod(_data->startLine[0]);  
}

int		HttpHeaderParser::_hexValue(char c) {
	if ('0' <= c && c <= '9')
        return c - '0';
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

void	HttpHeaderParser::_decodePercentEncoding(std::string& requestTarget) {
	for (std::size_t i = 0; i < requestTarget.size(); ++i) {
        if (requestTarget[i] != '%')
            continue;

        if (requestTarget.size() <= i + 2)
            throw BadRequest();

        int high = _hexValue(requestTarget[i + 1]);
        int low  = _hexValue(requestTarget[i + 2]);

        if (high == -1 || low == -1)
            throw BadRequest();

        requestTarget[i] = static_cast<char>((high << 4) | low);
        requestTarget.erase(i + 1, 2);
    }
}

void    HttpHeaderParser::_splitRequestTarget(std::string& requestTarget) {
    std::string::size_type pos = requestTarget.find( '?' );

    if ( pos != std::string::npos ) {
        _data->path = requestTarget.substr( 0, pos );
        _data->query = requestTarget.substr( pos + 1 );
    }
    else
        _data->path = requestTarget;
}

void	HttpHeaderParser::_normalizePath() {
    std::string result;
	size_t 		start = 0;	
    bool 		endsWithSlash = !_data->path.empty() && _data->path.back() == '/';
	
    result.reserve(_data->path.size());
    while (start < _data->path.size())
    {
        size_t end = _data->path.find('/', start);

        if (end == std::string::npos)
            end = _data->path.size();

        std::string_view part(_data->path.data() + start, end - start);

        if (part.empty() || part == ".") {
        }
        else if (part == "..") {
            if (result.empty())
                throw BadRequest();

            // remove previous directory
            size_t slash = result.find_last_of('/');

            if (slash == std::string::npos)
                result.clear();
            else
                result.erase(slash);
        }
        else {
            result.push_back('/');
            result.append(part);
        }

        start = end + 1;
    }

    if (endsWithSlash && result != "/")
        result.push_back('/');

    if (result.empty())
		_data->path = "/";
	else
		_data->path = std::move(result);
}

void	HttpHeaderParser::_validatePort(const std::string& port) {
	int value = 0;

    if (port.empty())
        throw BadRequest();

    for (char c : port) {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            throw BadRequest();
        value = value * 10 + (c - '0');
        if (65535 < value)
            throw BadRequest();
    }

    if (value == 0)
        throw BadRequest();
}

void	HttpHeaderParser::_validateHostName(std::string_view hostName) {
    if (hostName.empty()
	|| 253 < hostName.size()
	|| hostName.front() == '.'
	|| hostName.back() == '.')
		throw BadRequest();

    std::size_t start = 0;
    while (start < hostName.size()) {
        std::size_t end = hostName.find('.', start);
        if (end == std::string_view::npos)
            end = hostName.size();

        std::string_view label = hostName.substr(start, end - start);

        // Empty label (e.g. "example..com")
        if (label.empty())
            throw BadRequest();

        // RFC: max 63 characters per label
        if (63 < label.size())
            throw BadRequest();

        // Labels cannot start or end with '-'
        if (label.front() == '-' || label.back() == '-')
            throw BadRequest();

        // Only letters, digits and '-'
        for (unsigned char c : label) {
            if (!std::isalnum(c) && c != '-')
                throw BadRequest();
        }

        start = end + 1;
    }
}

static bool isIPv4(const std::string& value) {
	struct in_addr addr{};

    return inet_pton(AF_INET, value.c_str(), &addr) == 1;
}

static bool isIPv6(const std::string& value) {
    struct in6_addr addr{};

    return inet_pton(AF_INET6, value.c_str(), &addr) == 1;
}

void    HttpHeaderParser::_validateHostHeader(const std::string& host) {
    if (host.empty())
        throw BadRequest();

    // IPv6: [addr]:port
    if (host.front() == '[') {
        size_t close = host.find(']');

        if (close == std::string::npos || close == 1)
            throw BadRequest();

		_data->hostName = host.substr(1, close - 1);
		if (!isIPv6(_data->hostName))
			throw BadRequest();

        if (close + 1 < host.size()) {
            if (host[close + 1] != ':' || host.size() <= close + 2)
                throw BadRequest();

			_data->hostPort = host.substr(close + 2);
            _validatePort(_data->hostPort);
        }
        return;
    }

    // hostname / IPv4
    size_t colon = host.find(':');

	if (colon == std::string_view::npos)
		colon = host.size();

	_data->hostName = host.substr(0, colon);
	if (!isIPv4(_data->hostName))
		_validateHostName(_data->hostName);

	if (colon < host.size()) {
		if (host.size() <= colon + 1)
                throw BadRequest();
		_data->hostPort = host.substr(colon + 1);
		_validatePort(_data->hostPort);
	}
}

void	HttpHeaderParser::parse(char* buffer, std::size_t count) {
	_data->request.append(buffer, count);
    std::string::size_type start = 0;
    std::string::size_type pos;

    while ((pos = _data->request.find("\r\n", start)) != std::string::npos) {
        std::string_view line(_data->request.data() + start, pos - start);
        if (line.empty()) {
            if (_data->startLine[0].empty())
                throw BadRequest();
            _data->request.erase(0, pos + 2);
			_data->parseCompleted = true;
			_setMethod();
            return;
        }

        if (_data->startLine[0].empty()) {
            std::size_t fieldStart = 0;
            std::size_t fieldEnd = line.find(' ');
			if (fieldEnd == std::string_view::npos)
				throw BadRequest();
			_data->startLine[0] = line.substr(fieldStart, fieldEnd - fieldStart);

			fieldStart = fieldEnd + 1;
			fieldEnd = line.find(' ', fieldStart);
			if (fieldEnd == std::string_view::npos)
				throw BadRequest();
			_data->startLine[1] = line.substr(fieldStart, fieldEnd - fieldStart);

			fieldStart = fieldEnd + 1;
			if (line.size() <= fieldStart)
				throw BadRequest();
			_data->startLine[2] = line.substr(fieldStart);

            _checkProtocol();
            _decodePercentEncoding(_data->startLine[1]);
            _splitRequestTarget(_data->startLine[1]);
            _normalizePath();
        }
        else {
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
            if (key == "content-length") {
                if (_data->headers.find("content-length") != _data->headers.end()
				|| _data->headers.find("transfer-encoding") != _data->headers.end())
					throw BadRequest();
                if (!isAllDigits(value))
                    throw BadRequest();
            }
            else if (key == "transfer-encoding") {
				for (auto& x : value)
                	x = tolower(static_cast<unsigned char>(x));
				if (value == "chunked") {
					if (_data->headers.find("content-length")
					!= _data->headers.end())
						throw BadRequest();
				}
            }
            else if (key == "host")
                _validateHostHeader(value);

            _data->headers[key] = value;
        }

        start = pos + 2;
    }
	_data->request.erase(0, start);
}
