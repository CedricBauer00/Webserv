#include "../inc/HttpParser.hpp"
#include "../inc/Exceptions.hpp"

HttpParser::HttpParser() : _startLine(), _headers(), _body(), _contentLength( 0 ), _bodyLength( 0 ),_chunked( false )
{
    std::cout << "HttpParsing BEGIN" << std::endl;
}

void    HttpParser::setHeaders( std::string request )
{
    std::istringstream  iss( request );
    std::string         line;
    // int                 foundContlen = 0;
    bool                foundContlen = false;

    std::cout << GREEN << "request = " << request << RESET << std::endl;

    while ( std::getline( iss, line ) )
    {
        if ( !line.empty() && line.back() == '\r' )
            line.pop_back();
        // else
        //     break ; // fertig mit den headern?

        if ( line.empty() )
            break ;

        if ( _startLine.empty() )
        {
            setStartLine( line );
            for ( std::vector<std::string>::const_iterator it = _startLine.begin(); it != _startLine.end(); ++it )
            {
                std::cout << GREEN << "_startLine = " << *it << RESET << std::endl;
            }
            if ( checkStartLine() == false )
                throw std::runtime_error( "Invalid Host Header" );
        }
        else
        {
            std::string::size_type pos = line.find( ":" ); // vor ":" darf kein Space stehen
            if ( pos == std::string::npos || pos == 0 )
                throw std::runtime_error( "Bad Request" ); //400

            if ( line[ pos - 1 ] == ' ' )
                throw std::runtime_error( "Bad Request" ); //400

            std::string key = line.substr( 0, pos );
            std::string value = line.substr( pos + 1 );

            // key und value entweder komplett lowercase oder uppercase machen, wegen einheitlichem handling - case sensitive 
            for ( auto& x : key )
                x = tolower( static_cast<unsigned char>( x ) );
            for ( auto& x : value )
                x = tolower( static_cast<unsigned char>( x ) );

            value = trim( value );
            if ( key == "content-length" && foundContlen == false ) // need to be checked when there ist post
            {
                foundContlen = true;
                if ( isAllDigits( value ) == false )
                    throw std::runtime_error( "Bad Request" ); //400
                _contentLength = stoi( value );
                // foundContlen++;
            }
            else if ( key == "transfer-encoding" && value == "chunked")
                _chunked = true;// body endet bei \0\r\n
            
            // if ( foundContlen > 1)
            //     throw std::runtime_error( "Redundant header" );

            if ( _chunked  && foundContlen )
                throw std::runtime_error( "Bad Request" ); //400

            _headers[ key ] = value;
        }
    }
    std::cout << BLUE << "Map printing" << RESET << std::endl;
    for ( const auto& pair : _headers )
    {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
}

void    HttpParser::setStartLine( std::string line )
{
    std::istringstream  iss( line );
    std::string         token;
    std::cout << GREEN << "line = " << line << std::endl;
    while ( iss >> token )
        _startLine.push_back( token );
}

bool    HttpParser::checkStartLine()
{
    if ( _startLine.size() != 3 )
        throw std::runtime_error( "" );
    if ( _startLine[ 0 ][ 0 ] == '/' )
        throw BadRequest();
    if ( _startLine[ 0 ] != "GET" && _startLine[ 0 ] != "POST" && _startLine[ 0 ] != "DELETE" )
        throw std::runtime_error( "405" );
    if ( _startLine[ 2 ].substr( 0, 4 ) != "HTTP" )
        throw std::runtime_error( "400" );
    if ( _startLine[ 2 ] != "HTTP/1.0" && _startLine[ 2 ] != "HTTP/1.1" )
        throw std::runtime_error( "505" );
    
    return true;
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
        if ( !std::isdigit( static_cast<unsigned char>(*it) ) )
            return false;
    }
    return true;
}

void    HttpParser::setBody()
{
    //check if contentlength and body length are the same
    // read request body into _body variable after headers were parsed correctly

}

HttpParser::~HttpParser()
{
    std::cout << "HttpParsing END" << std::endl;
}

// Errorcodes fuer pailed parsing: incorrect syntax 400 Bad Request