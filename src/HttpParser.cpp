#include "../inc/HttpParser.hpp"
#include "../inc/Exceptions.hpp"

HttpParser::HttpParser() {}

HttpParser::HttpParser( std::string reqeust ) : _startLine(), _headers(), _body(), _bodyLength( 0 ), _foundContlen( false ), _foundHost( false ), _contentLength( 0 ), _chunked( false ), _iss( reqeust ), _method( METHOD_GET ), _isCgiFile( false )
{
    std::cout << "\n--- HttpParsing BEGIN ---\n" << std::endl;
}

HttpParser::~HttpParser() { std::cout << RED << "--- HttpParsing END ---" << RESET << std::endl; }

void    HttpParser::setHeaders()
{
    std::string line;

    int whichline = 0;
    while ( std::getline( _iss, line ) )
    {
        whichline++;
        std::cout << GREEN << line << RESET << std::endl;
        if ( !line.empty() && line.back() == '\r' )
            line.pop_back();

        if ( line.empty() )
            break ;

        if ( _startLine.empty() )
        {
            setStartLine( line );
            checkStartLine();
            setMethod();
            setUri();
            checkCgiExtension();
        }
        else
        {
            std::string::size_type pos = line.find( ":" ); 
            if ( pos == std::string::npos || pos == 0 )
                throw BadRequest();

            if ( line[ pos - 1 ] == ' ' ) // vor ":" darf kein Space stehen
                throw BadRequest();

            std::string key = line.substr( 0, pos );
            std::string value = line.substr( pos + 1 );

            // key und value entweder komplett lowercase oder uppercase machen, wegen einheitlichem handling - case sensitive 
            for ( auto& x : key )
                x = tolower( static_cast<unsigned char>( x ) );
            for ( auto& x : value )
                x = tolower( static_cast<unsigned char>( x ) );

            value = trim( value );
            key = trim( key );
            if ( key == "content-length" && _foundContlen == false ) // need to be checked when there ist post
            {
                _foundContlen = true;
                if ( isAllDigits( value ) == false )
                    throw BadRequest(); // fall back to content length from config
                _contentLength = static_cast<std::size_t>( std::stoi( value ) );
                // check on content length from config file
            }
            else if ( key == "transfer-encoding" && value == "chunked")
                _chunked = true;// body endet bei \0\r\n
            
            if ( _chunked  && _foundContlen )
                throw BadRequest();
            
            if ( key == "host" )
                checkHostHeader( value );
            
            _headers[ key ] = value;
        }
    }
    if ( _foundHost == false ) // oder default server - meist erster Serverblock
        throw BadRequest(); // anscheinend muss! dann default server; keine Bad Request!

    // std::cout << "found Host Header:" << _foundHost << " in line: "<< BLUE << whichline << RESET << std::endl;
    std::cout << BLUE << "- Map printing- \n" << std::endl;
    for ( const auto& pair : _headers )
    {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    std::cout << "\n- Map End -\n" << RESET << std::endl;
}

void    HttpParser::setStartLine( std::string line )
{
    std::istringstream  iss( line );
    std::string         token;

    while ( iss >> token )
        _startLine.push_back( token );
}

void    HttpParser::checkStartLine() // eventuell direkt Execution instance createn, die URI speichert
{
    if ( _startLine.size() != 3 )
        throw BadRequest();
    if ( _startLine[ 0 ][ 0 ] == '/' )
        throw BadRequest();
    if ( _startLine[ 0 ] != "GET" && _startLine[ 0 ] != "POST" && _startLine[ 0 ] != "DELETE" )
        throw MethodNotAllowed();    
    if ( _startLine[ 2 ].substr( 0, 4 ) != "HTTP" )
        throw BadRequest();
    if ( _startLine[ 2 ] != "HTTP/1.0" && _startLine[ 2 ] != "HTTP/1.1" )
        throw HttpVersionNotSupported();
}

void    HttpParser::setUri()
{
    _uri = _startLine[ 1 ];
}


void    HttpParser::setMethod() // eventuell hier Execution class instance createn, die die Method selbst speichert
{
    std::cout << "startline:" << _startLine[ 0 ] << std::endl;
    if ( _startLine[ 0 ] == "GET" )
        _method = METHOD_GET;
    else if ( _startLine[ 0 ] == "POST" )
        _method = METHOD_POST;
    else if ( _startLine[ 0 ] == "DELETE" )
        _method = METHOD_DELETE;
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
    std::string line;

    if ( _chunked == true )
    {
        std::cout << "CHUNKED ENCODING" << std::endl;
        // chunked encoding
        // until body lenght == 0; -> End of chunked encoding
    }
    else
    {
        while ( std::getline( _iss, line ) )
        {
            _body.append( line + "\n" );
        }
        
        // max body size checken
        if ( _foundContlen )
        {
            if ( _contentLength > static_cast<std::size_t>( MAX_BODY_SIZE ) )
                throw PayloadTooLarge();
            if ( _body.size() != _contentLength )
                throw BadRequest();
            // if ( _body.size() < _contentLength )
            //     throw RequestTimeout();
            
        }
        else if ( _body.size() > static_cast<std::size_t>( MAX_BODY_SIZE ) )
            throw PayloadTooLarge();
            
    }
    //check if contentlength and body length are the same
    // read request body into _body variable after headers were parsed correctly
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

void    HttpParser::checkHostHeader( std::string value )
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

void    HttpParser::checkCgiExtension()
{
    size_t size = _uri.size(); 

    if ( size >= 3 && _uri.compare( size - 3, 3, ".py" ) == 0 )
    {
        _isCgiFile = true;
    }
}

bool    isInRange( int num, int min, int max )
{
    return ( num >= min && num <= max );
}

whichMethod  HttpParser::getMethod() const
{
    return _method;
}

std::string     HttpParser::getUri()
{
    return _uri;
}

std::string    HttpParser::getBody() const
{
    return _body;
}

std::string     HttpParser::getHostName()
{
    return _hostName;
}

std::string     HttpParser::getHostPort()
{
    return _hostPort;
}

bool            HttpParser::isCgifile()
{
    return _isCgiFile;
}

// test for carriage return
// printf 'GET /Something HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490