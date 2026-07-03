#include "../inc/AHttpParser.hpp"
#include "../inc/Exceptions.hpp"

AHttpParser::AHttpParser() 
: _startLine(), _headers(), _body(), _bodyLength(0), _headStopReceived(false), _bodyStopReceived(false),
_foundContlen( false ), _foundHost( false ), _contentLength( 0 ),
_chunked( false ), _request(), _method( METHOD_GET ),
_currentChunkSize(0), _waitingForChunkData(false), _waitingForLastChunkCRLF(false) {
}

AHttpParser::AHttpParser(const std::string& request ) 
: _startLine(), _headers(), _body(), _bodyLength(0), _headStopReceived(false), _bodyStopReceived(false),
_foundContlen( false ), _foundHost( false ), _contentLength( 0 ),
_chunked( false ), _request(request), _method( METHOD_GET ),
_currentChunkSize(0), _waitingForChunkData(false), _waitingForLastChunkCRLF(false) {
}

AHttpParser::AHttpParser(AHttpParser&& other) noexcept
: _startLine( std::move( other._startLine ) )
, _headers( std::move( other._headers ) )
, _httpVersion( std::move( other._httpVersion ) )
, _path( std::move( other._path ) )
, _query( std::move( other._query ) )
, _body( std::move( other._body ) )
, _bodyLength( other._bodyLength )
, _headStopReceived( other._headStopReceived )
, _bodyStopReceived( other._bodyStopReceived )
, _foundContlen( other._foundContlen )
, _foundHost( other._foundHost )
, _contentLength( other._contentLength )
, _chunked( other._chunked )
, _request( std::move( other._request ) )
, _hostPort( std::move( other._hostPort ) )
, _hostName( std::move( other._hostName ) )
, _method( other._method )
, _methodMask( other._methodMask )
, _currentChunkSize(other._currentChunkSize)
, _waitingForChunkData(other._waitingForChunkData)
, _waitingForLastChunkCRLF(other._waitingForLastChunkCRLF)
, _internalRedirect(other._internalRedirect)
{
	std::cout << "AHttpParser move constructor called" << std::endl;
}

AHttpParser::~AHttpParser() {
}

void	AHttpParser::parseBody(char* buffer, std::size_t count)
{
	_request.append(buffer, count);
	if (_chunked) {
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
				if(MAX_BODY_SIZE < _body.size() + _currentChunkSize)
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
	
	if (_foundContlen) {
		std::size_t remaining = _contentLength - _body.size();
		std::size_t toCopy = std::min(remaining, _request.size());

		_body.append(_request.data(), toCopy);
		_request.erase(0, toCopy);
		if (_body.size() == _contentLength)
			_bodyStopReceived = true;
		return;
	}

	if (_startLine[2] != "HTTP/1.0")
    	throw BadRequest();
	_body.append(_request);
	if (MAX_BODY_SIZE < _body.size())
		throw PayloadTooLarge();
	_request.clear();
	return;
}

void    AHttpParser::_checkStartLine() // eventuell direkt Execution instance createn, die URI speichert
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
        throw MethodNotImplemented();    
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

void    AHttpParser::_setMethod() // eventuell hier Execution class instance createn, die die Method selbst speichert
{
    setMethod(_startLine[0]);  
}

void    AHttpParser::setMethod(const std::string& method) // eventuell hier Execution class instance createn, die die Method selbst speichert
{
    auto it = methodMap.find(method);
    if (it == methodMap.end())
		throw MethodNotImplemented();
	_methodMask = it->second.first;
	_method = it->second.second;    
}

std::string AHttpParser::trim( const std::string& value )
{
    std::string::size_type start = 0;
    while ( start < value.size() && std::isspace( static_cast<unsigned char>( value[ start ] ) ) )
        ++start;

    std::string::size_type end = value.size();
    while ( end > start && std::isspace( static_cast<unsigned char>( value[ end - 1 ] ) ) )
        --end;
    return ( value.substr( start, end - start ) );
}

void    AHttpParser::validatePort( std::string port )
{
    if ( port.empty() || !isAllDigits( port ) )
        throw BadRequest();
    int _hostPort = static_cast<std::size_t>( std::stoi( port ) );
    if ( !isInRange( _hostPort, 1, 65535 ) ) // 65534 because 65535 is not permitted! // 65535 ist valid TCP/UDP port range
        throw BadRequest();
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

void    AHttpParser::checkHostHeader(const std::string& value )
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

void	AHttpParser::setRedirectPath(std::string&& redirectPath) {
	_path = std::move(redirectPath);
	_internalRedirect = true;
}

const std::vector<std::string>&    AHttpParser::getStartLine() const
{
	return _startLine;
}

const std::string&	AHttpParser::getQuery() const
{
	return _query;
}

const std::unordered_map<std::string, std::string>&    AHttpParser::getHeaders() const
{
	return _headers;
}

const std::string&    AHttpParser::getBody() const
{
    return _body;
}

const std::string&	AHttpParser::getMethodStr() const
{
	return _startLine[0];
}

method  AHttpParser::getMethod() const
{
    return _method;
}

unsigned int    AHttpParser::getMethodMask() const
{
    return _methodMask;
}

const std::string&     AHttpParser::getHostName() const
{
    return _hostName;
}

const std::string&     AHttpParser::getHostPort() const
{
    return _hostPort;
}

const std::string&     AHttpParser::getPath() const
{
    return _path;
}

const std::string&	AHttpParser::getHttp() const 
{
	return  _startLine[2];
}

bool			AHttpParser::headStopReceived() const {
	return _headStopReceived;
}

bool			AHttpParser::bodyStopReceived() const {
	return _bodyStopReceived;
}

bool	AHttpParser::headerHasContlen() const {
	return _foundContlen;
}

std::size_t	AHttpParser::getContlen() const {
	return _contentLength;
};

bool	AHttpParser::isHTTP1p0() const {
	return _startLine[2] == "HTTP/1.0";
}

void    AHttpParser::_decodeRequestTarget(std::string& requestTarget)
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

void    AHttpParser::_splitRequestTarget(std::string& requestTarget) {
    std::string::size_type pos = requestTarget.find( '?' );

    if ( pos != std::string::npos )
    {
        _path = requestTarget.substr( 0, pos );
        _query = requestTarget.substr( pos + 1 );
    }
    else
        _path = requestTarget;
}

void    AHttpParser::_normalizePath() {
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
        nPath += '/';
        nPath += wholePath[ i ];
    }
    if ( endsWithSlash )
        nPath += '/';
    _path = std::move(nPath);
}

bool	AHttpParser::isInRange( int num, int min, int max )
{
    return ( num >= min && num <= max );
}

bool	AHttpParser::isIpv6Char( char c )
{
    return ( c == ':' || std::isxdigit( static_cast<unsigned char>( c ) ) );
}
