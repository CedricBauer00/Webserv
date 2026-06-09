#include "../inc/HttpParser.hpp"
#include "../inc/Exceptions.hpp"

HttpParser::HttpParser() 
: _startLine(), _headers(), _body(), _bodyLength( 0 ), _complHead( false ),
_foundContlen( false ), _foundHost( false ), _contentLength( 0 ),
_chunked( false ), _request(), _method( METHOD_GET ) {
}

HttpParser::HttpParser(const std::string& request ) 
: _startLine(), _headers(), _body(), _bodyLength( 0 ), _complHead( false ),
_foundContlen( false ), _foundHost( false ), _contentLength( 0 ),
_chunked( false ), _request(request), _method( METHOD_GET ) {
}

HttpParser::HttpParser(HttpParser&& other) noexcept
: _startLine( std::move( other._startLine ) )
, _headers( std::move( other._headers ) )
, _httpVersion( std::move( other._httpVersion ) )
, _path( std::move( other._path ) )
, _query( std::move( other._query ) )
, _body( std::move( other._body ) )
, _bodyLength( other._bodyLength )
, _foundContlen( other._foundContlen )
, _foundHost( other._foundHost )
, _contentLength( other._contentLength )
, _chunked( other._chunked )
, _request( std::move( other._request ) )
, _hostPort( std::move( other._hostPort ) )
, _hostName( std::move( other._hostName ) )
, _method( other._method )
, _reqMethodMask( other._reqMethodMask )
{
}

HttpParser::~HttpParser() {
    std::cout << RED << "--- HttpParsing END ---" << RESET << std::endl; }

// void    HttpParser::parse()
// {
//     std::istringstream requestStream(_request);
//     std::string line;

//     while (std::getline(requestStream, line))
//     {
//         if (!line.empty() && line.back() == '\r')
//             line.pop_back();
//         if (line.empty())
//             break;
//         parseHead(line);
//     }
// }

bool    HttpParser::parseHead(std::string::size_type pos)
{
    std::string_view line(_request.data(), pos);
    if (line.empty() || line.back() != '\r')
        throw BadRequest();
    line.remove_suffix(1);

    if (line.empty())
    {
        _request.erase(0, pos + 1);
		for ( const auto& pair : _headers )
		{
			std::cout << BLUE << pair.first << " : "
			<< pair.second << RESET << std::endl;
		}
        return true;
    }

    std::cout << GREEN << std::string(line) << RESET << std::endl;

    if (_startLine.empty())
    {
        std::size_t start = 0;
        std::size_t end = line.find(' ');
        while (end != std::string_view::npos)
        {
            _startLine.emplace_back(line.substr(start, end - start));
            start = end + 1;
            end = line.find(' ', start);
        }
        _startLine.emplace_back(line.substr(start));
        _checkStartLine();
        _setMethod();
        _decodeRequestTarget(_startLine[1]);
        _splitRequestTarget(_startLine[1]);
        _normalizePath();
		_complHead = true;
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
        for (auto& x : value)
            x = tolower(static_cast<unsigned char>(x));

        value = trim(value);
        key = trim(key);
        if (key == "content-length" && _foundContlen == false)
        {
            _foundContlen = true;
            if (!isAllDigits(value))
                throw BadRequest();
            _contentLength = static_cast<std::size_t>(std::stoi(value));
        }
        else if (key == "transfer-encoding" && value == "chunked")
            _chunked = true;

        if (_chunked && _foundContlen)
            throw BadRequest();

        if (key == "host")
            checkHostHeader(value);

        _headers[key] = value;
    }
    // if ( _foundHost == false ) // oder default server - meist erster Serverblock
    //     throw BadRequest(); // anscheinend muss! dann default server; keine Bad Request!

    // std::cout << "found Host Header:" << _foundHost << " in line: "<< BLUE << whichline << RESET << std::endl;
	_request.erase(0, pos + 1);
    return false;
}

void    HttpParser::_checkStartLine() // eventuell direkt Execution instance createn, die URI speichert
{
    if ( _startLine.size() != 3 )
    {
        std::cout << RED << "not 3 args in starline" << RESET << std::endl;////
        throw BadRequest();
    }
    if ( _startLine[ 0 ][ 0 ] == '/' )
    {
        std::cout << RED << "'/' as first char" << RESET << std::endl;////
        throw BadRequest();
    }
    if ( _startLine[ 0 ] != "GET" && _startLine[ 0 ] != "POST" && _startLine[ 0 ] != "DELETE" )
        throw MethodNotAllowed();    
    if ( _startLine[ 2 ].substr( 0, 4 ) != "HTTP" )
    {
        std::cout << RED << "Not an HTTP protocol" << RESET << std::endl;////
        throw BadRequest();
    }
    if ( _startLine[ 2 ] != "HTTP/1.0" && _startLine[ 2 ] != "HTTP/1.1" )
    {
        std::cout << RED << "HTTP Version not supported" << RESET << std::endl; ////
        throw HttpVersionNotSupported();
    }
}

void    HttpParser::_setMethod() // eventuell hier Execution class instance createn, die die Method selbst speichert
{
    std::cout << "startline:" << _startLine[ 0 ] << std::endl;
    if ( _startLine[ 0 ] == "GET" )
        _method = METHOD_GET;
    else if ( _startLine[ 0 ] == "POST" )
        _method = METHOD_POST;
    else if ( _startLine[ 0 ] == "DELETE" )
        _method = METHOD_DELETE;
    _reqMethodMask = m.at(_startLine[ 0 ]);
    std::cout << "_method:" << _method << std::endl;
    
}

std::string HttpParser::trim( const std::string& value )
{
    std::string::size_type start = 0;
    while ( start < value.size() && std::isspace( static_cast<unsigned char>( value[ start ] ) ) )
        ++start;

    std::string::size_type end = value.size();
    while ( end > start && std::isspace( static_cast<unsigned char>( value[ end - 1 ] ) ) )
        --end;
    return ( value.substr( start, end - start ) );
}

bool    HttpParser::isAllDigits( const std::string& word )
{
    for ( std::string::const_iterator it = word.begin(); it != word.end(); ++it )
    {
        if ( !std::isdigit( static_cast<unsigned char>( *it ) ) )
            return ( false );
    }
    return ( true );
}

void    HttpParser::setBody()
{
    const std::string::size_type headerEnd = _request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        throw BadRequest();

    const std::string remaining = _request.substr(headerEnd + 4);

    if (_chunked == true)
    {
        (void)remaining;
        return;
    }
    if (_foundContlen)
    {
        if (static_cast<std::size_t>(MAX_BODY_SIZE) < _contentLength)
            throw PayloadTooLarge();
        if (remaining.size() < _contentLength)
            throw BadRequest();
        _body.assign(remaining.begin(), remaining.begin() + static_cast<std::ptrdiff_t>(_contentLength));
        return;
    }

    _body = remaining;
    if (_body.size() > MAX_BODY_SIZE)
        throw PayloadTooLarge();
}

void    HttpParser::validatePort( std::string port )
{
    if ( port.empty() || !isAllDigits( port ) )
        throw BadRequest();
    int _hostPort = static_cast<std::size_t>( std::stoi( port ) );
    if ( !isInRange( _hostPort, 1, 65535 ) ) // 65534 because 65535 is not permitted! // 65535 ist valid TCP/UDP port range
        throw BadRequest();
}

bool    isIpv6Char( char c )
{
    return ( c == ':' || std::isxdigit( static_cast<unsigned char>( c ) ) );
}

std::string validateHostName( const std::string &hostName )
{
    if ( hostName.front() == '.' ||
            hostName.front() == '-' ||
            hostName.back() == '.' ||
            hostName.back() == '-')
        throw BadRequest();
    for ( auto x : hostName )
    {
        if ( !( std::isalnum( static_cast<unsigned char>( x ) ) || x == '.' || x == '-' ) )
            throw BadRequest();
    }
    return hostName;
}

void    HttpParser::checkHostHeader(const std::string& value )
{
    if ( value.empty() )
        throw BadRequest();

    if ( value.find( ' ' ) != std::string::npos )
        throw BadRequest();
    if ( value.find( "http://" ) == 0 )
        throw BadRequest();

    if ( value[ 0 ] != '[' )
    {
        if ( value.find( '[' ) != std::string::npos || value.find( ']' ) != std::string::npos )
            throw BadRequest();

        std::string::size_type first = value.find( ':' );
        std::string::size_type last = value.rfind( ':' );
    
        if ( first == std::string::npos )
            _hostName = validateHostName( value );
        else if ( first != last ) // muss man hier auch first != std::string::npos checken? // warum soll ich hier auch auf first != std::string::npos checken?
            throw BadRequest();
        else        
        {
            _hostName = validateHostName( value.substr( 0, first ) );
            _hostPort = value.substr( first + 1 );
            validatePort( _hostPort );
        }
    }
    else if ( value[ 0 ] == '[' )
    {
        std::string::size_type secBrace = value.find( ']' );
        if ( secBrace == std::string::npos )
            throw BadRequest();
        if ( secBrace == 1 )
            throw BadRequest();

        std::string inBetween = value.substr( 1, secBrace - 1 );
        if ( inBetween.empty() )
            throw BadRequest();

        for ( size_t i = 0; i < inBetween.size(); ++i )
        {
            if ( !isIpv6Char( inBetween[ i ] ) )
                throw BadRequest();
        }

        size_t colPos = inBetween.find( "::" );
        if ( colPos != std::string::npos )
        {
            if ( inBetween.find( "::", colPos + 2 ) != std::string::npos )
                throw BadRequest();
        }

        if ( secBrace + 1 < value.size() ) // es gibt noch port/zeichen danach
        {
            if ( value[ secBrace + 1 ] != ':' )
                throw BadRequest();
            std::string portStr = value.substr( secBrace + 2 ); // weil "]:"
            validatePort( portStr );    
        }
    }
                // NOT DONE YET!!!! muss fixen und auch noch Port und HostName korrekt abspeichern 

    _foundHost = true;
}
// [:::::example.com]   faield: wird gepassed!
// [example.com]        faield: wird gepassed!
// example.com]
// printf 'GET /legacy/location/ HTTP/1.1\r\nHOST: ccc[]cc\r\nHEAEDER1: A A \r\n\r\n' | nc 127.0.0.2 3490   
// ~$ printf 'GET /legacy/location/ HTTP/1.1\r\nHOST: [ccc]\r\nHEAEDER1: A A \r\n\r\n' | nc 127.0.0.2 3490
// ~$ printf 'GET /legacy/location/ HTTP/1.1\r\nHOST: [:ccc]\r\nHEAEDER1: A A \r\n\r\n' | nc 127.0.0.2 3490
// ~$ printf 'GET /legacy/location/ HTTP/1.1\r\nHOST: [::ccc]\r\nHEAEDER1: A A \r\n\r\n' | nc 127.0.0.2 3490
// ~$ printf 'GET /legacy/location/ HTTP/1.1\r\nHOST: [::]:6553\r\nHEAEDER1: A A \r\n\r\n' | nc 127.0.0.2 3490

bool    isInRange( int num, int min, int max )
{
    return ( num >= min && num <= max );
}

whichMethod  HttpParser::getMethod() const
{
    return _method;
}

unsigned int    HttpParser::getReqMethod() const
{
    return _reqMethodMask;
}

const std::string&    HttpParser::getBody() const
{
    return _body;
}

const std::string&     HttpParser::getHostName() const
{
    return _hostName;
}

const std::string&     HttpParser::getHostPort() const
{
    return _hostPort;
}

const std::string&     HttpParser::getPath() const
{
    return _path;
}

std::string&	HttpParser::getRequest() {
	return _request;
}

bool			HttpParser::isComplHead() const {
	return _complHead;
}

void    HttpParser::_decodeRequestTarget(std::string& requestTarget)
{
    for ( size_t i = 0; i < requestTarget.size(); ++i )
    {
        if ( requestTarget[ i ] == '%' && i + 2 < requestTarget.size() )
        {
            std::string hex = requestTarget.substr( i + 1, 2 );
            char c = static_cast<char>( std::strtol( hex.c_str(), 0, 16 ) );
            requestTarget.replace( i, 3, 1, c );
        }
    }
}

void    HttpParser::_splitRequestTarget(std::string& requestTarget) {
    std::string::size_type pos = requestTarget.find( '?' );

    if ( pos != std::string::npos )
    {
        _path = requestTarget.substr( 0, pos );
        _query = requestTarget.substr( pos + 1 );
    }
    else
        _path = requestTarget;
}

void    HttpParser::_normalizePath() {
    std::istringstream iss( _path );
    std::vector<std::string> wholePath;
    std::string partStr;
    std::string nPath;

    bool endsWithSlash = !_path.empty() && _path.back() == '/';
    while ( getline( iss, partStr, '/' ) )
    {
        if ( partStr.empty() || partStr == "." ) // "." - dieses Verzeichnis
            continue ;
        if ( partStr == ".." ) // Traversal-Check 
        {
            if ( wholePath.empty() )
                throw BadRequest();
                
            wholePath.pop_back(); // one directory out  
        }
        else
            wholePath.push_back( partStr );
    }
    for ( size_t i = 0; i < wholePath.size(); ++i )
    {
        nPath = "/" + nPath;
        nPath += wholePath[ i ];
    }
    if ( endsWithSlash )
        nPath += '/';
    std::cout << "Normalized path: " << nPath << std::endl;
    _path = std::move(nPath);
}

// bool            HttpParser::isCgifile()
// {
//     return _isCgiFile;
// }
// test for carriage return
// printf 'GET /Something HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490