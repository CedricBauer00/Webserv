#include "../inc/AHttpParser.hpp"
#include "../inc/Exceptions.hpp"

AHttpParser::AHttpParser() : _data(std::make_unique<data>()) {
}

// AHttpParser::AHttpParser(const std::string& request ) 
// : _request(request) {
// }

AHttpParser::AHttpParser(AHttpParser&& other) noexcept
: _data(std::move(other._data)) {
	std::cout << "AHttpParser move constructor called" << std::endl;
}

AHttpParser::AHttpParser(AHttpParser&& other, std::size_t max_size) noexcept
: AHttpParser(std::move(other)) {
    _data->max_size = max_size; 
}

AHttpParser::~AHttpParser() {
}

void    AHttpParser::_checkStartLine() {
    if ( _data->startLine.size() != 3 )
    {
        std::cout << RED << "not 3 args in starline" << RESET << std::endl;
        throw BadRequest();
    }
    if ( _data->startLine[ 0 ][ 0 ] == '/' )
    {
        std::cout << RED << "'/' as first char" << RESET << std::endl;
        throw BadRequest();
    }
    if ( _data->startLine[ 0 ] != "GET"
    && _data->startLine[ 0 ] != "POST"
    && _data->startLine[ 0 ] != "DELETE" )
        throw MethodNotImplemented();    
    if ( _data->startLine[ 2 ].substr( 0, 4 ) != "HTTP" )
    {
        std::cout << RED << "Not an HTTP protocol" << RESET << std::endl;
        throw BadRequest();
    }
    if ( _data->startLine[ 2 ] != "HTTP/1.0"
    && _data->startLine[ 2 ] != "HTTP/1.1" )
    {
        std::cout << RED << "HTTP Version not supported" << RESET << std::endl;
        throw HttpVersionNotSupported();
    }
}

void    AHttpParser::_setMethod() {
    setMethod(_data->startLine[0]);  
}

void    AHttpParser::setMethod(const std::string& method) {
    auto it = methodMap.find(method);
    if (it == methodMap.end())
		throw MethodNotImplemented();
	_data->methodMask = it->second.first;
	_data->method = it->second.second;    
}

std::string AHttpParser::trim( const std::string& value ) {
    std::string::size_type start = 0;
    while ( start < value.size() && std::isspace( static_cast<unsigned char>( value[ start ] ) ) )
        ++start;

    std::string::size_type end = value.size();
    while ( end > start && std::isspace( static_cast<unsigned char>( value[ end - 1 ] ) ) )
        --end;
    return ( value.substr( start, end - start ) );
}

void    AHttpParser::validatePort( std::string port ) {
    if ( port.empty() || !isAllDigits( port ) )
        throw BadRequest();
    int _hostPort = static_cast<std::size_t>( std::stoi( port ) );
    if ( !isInRange( _hostPort, 1, 65535 ) ) // 65534 because 65535 is not permitted! // 65535 ist valid TCP/UDP port range
        throw BadRequest();
}

std::string validateHostName( const std::string &hostName ) {
    if ( hostName.front() == '.'
	|| hostName.front() == '-'
	|| hostName.back() == '.'
	|| hostName.back() == '-')
        throw BadRequest();
    for ( auto x : hostName )
    {
        if ( !( std::isalnum( static_cast<unsigned char>( x ) ) || x == '.' || x == '-' ) )
            throw BadRequest();
    }
    return hostName;
}

void    AHttpParser::checkHostHeader(const std::string& value ) {
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
            _data->hostName = validateHostName( value );
        else if ( first != last ) // muss man hier auch first != std::string::npos checken? // warum soll ich hier auch auf first != std::string::npos checken?
            throw BadRequest();
        else        
        {
            _data->hostName = validateHostName( value.substr( 0, first ) );
            _data->hostPort = value.substr( first + 1 );
            validatePort( _data->hostPort );
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
    // _foundHost = true;
}

void	AHttpParser::setRedirectPath(std::string&& redirectPath) {
	_data->path = std::move(redirectPath);
	_data->internalRedirect = true;
}

const std::vector<std::string>&    AHttpParser::getStartLine() const {
	return _data->startLine;
}

const std::string&	AHttpParser::getQuery() const {
	return _data->query;
}

const std::unordered_map<std::string, std::string>&    AHttpParser::getHeaders() const {
	return _data->headers;
}

const std::string&    AHttpParser::getBody() const {
    return _data->body;
}

const std::string&	AHttpParser::getMethodStr() const {
	return _data->startLine[0];
}

method  AHttpParser::getMethod() const {
    return _data->method;
}

unsigned int    AHttpParser::getMethodMask() const {
    return _data->methodMask;
}

const std::string&     AHttpParser::getHostName() const {
    return _data->hostName;
}

const std::string&     AHttpParser::getHostPort() const {
    return _data->hostPort;
}

const std::string&     AHttpParser::getPath() const {
    return _data->path;
}

const std::string&	AHttpParser::getHttp() const {
	return  _data->startLine[2];
}

bool	AHttpParser::parseCompleted() const {
	return _data->parseCompleted;
}

void	AHttpParser::unsetParseCompleted() {
	_data->parseCompleted = false;
}

bool	AHttpParser::headerHasChunked() const {
	return _data->chunked;
}

bool	AHttpParser::headerHasContlen() const {
	return _data->foundContlen;
}

std::size_t	AHttpParser::getContlen() const {
	return _data->contentLength;
};

bool	AHttpParser::http1p0Ended() {
	if (_data->startLine[2] == "HTTP/1.0") {
		_data->parseCompleted = true;
		return true;
	}
	return false;
}

void    AHttpParser::_decodeRequestTarget(std::string& requestTarget) {
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

    if ( pos != std::string::npos ) {
        _data->path = requestTarget.substr( 0, pos );
        _data->query = requestTarget.substr( pos + 1 );
    }
    else
        _data->path = requestTarget;
}

void    AHttpParser::_normalizePath() {
    std::istringstream iss( _data->path );
    std::vector<std::string> wholePath;
    std::string partStr;
    std::string nPath;

    bool endsWithSlash = !_data->path.empty() && _data->path.back() == '/';
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
    _data->path = std::move(nPath);
}

bool	AHttpParser::isInRange( int num, int min, int max ) {
    return ( min <= num && num <= max );
}

bool	AHttpParser::isIpv6Char( char c ) {
    return ( c == ':' || std::isxdigit( static_cast<unsigned char>( c ) ) );
}
