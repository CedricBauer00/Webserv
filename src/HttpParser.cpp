#include "../inc/HttpParser.hpp"
#include "../inc/Exceptions.hpp"

HttpParser::HttpParser() {}

HttpParser::HttpParser( std::string reqeust ) : _startLine(), _headers(), _body(), _bodyLength( 0 ), _foundContlen( false ), _contentLength( 0 ), _chunked( false ), _iss( reqeust ), _method( METHOD_GET )
{
    std::cout << "HttpParsing BEGIN" << std::endl;
}

void    HttpParser::setHeaders()
{
    // std::istringstream  iss( request );
    std::string         line;

    // initIss( request );
    // std::cout << GREEN << "request = " << request << RESET << std::endl;
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
            // for ( std::vector<std::string>::const_iterator it = _startLine.begin(); it != _startLine.end(); ++it )
            // {
            //     std::cout << GREEN << "_startLine = " << *it << RESET << std::endl;
            // }
            checkStartLine();
            setMethod();
            setUri();
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

            _headers[ key ] = value;
        }
    }
    std::cout << BLUE << whichline << RESET << std::endl;
    std::cout << BLUE << "Map printing" << std::endl;
    for ( const auto& pair : _headers )
    {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    std::cout << "Map End" << RESET << std::endl;
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
    if ( _startLine[ 0 ] == "GET" )
        _method = METHOD_GET;
    else if ( _startLine[ 0 ] == "POST" )
        _method = METHOD_POST;
    else if ( _startLine[ 0 ] == "DELETE" )
        _method = METHOD_DELETE;
}

std::string HttpParser::trim( const std::string& value )
{
    std::string::size_type start = 0;
    while ( start < value.size() && std::isspace( static_cast<unsigned char>( value[ start ] ) ) )
        ++start;

    std::string::size_type end = value.size();
    while ( end > start && std::isspace( static_cast<unsigned char>( value[ end - 1 ] ) ) )
        --end;
    return value.substr( start, end - start );
}

bool    HttpParser::isAllDigits( const std::string& word )
{
    for ( std::string::const_iterator it = word.begin(); it != word.end(); ++it )
    {
        if ( !std::isdigit( static_cast<unsigned char>( *it ) ) )
            return false;
    }
    return true;
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

Method HttpParser::getMethod() const
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

HttpParser::~HttpParser()
{
    std::cout << "HttpParsing END" << std::endl;
}

// Errorcodes fuer pailed parsing: incorrect syntax 400 Bad Request

// test for carriage return
// printf 'GET /Something HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490

