#include "../inc/HttpParser.hpp"

HttpParser::HttpParser() : _startLine(), _headers(), _body()
{
    std::cout << "HttpParsing BEGIN" << std::endl;
}

void    HttpParser::setHeaders( std::string request )
{
    std::istringstream iss( request );
    std::string line;

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
                std::cout << GREEN << "_startLine = " << *it << RESET << std::endl;
        }
        else
        {
            std::string::size_type pos = line.find( ":" ); // vor ":" darf kein Space stehen
            if ( pos == std::string::npos )
            {
                throw std::runtime_error("400 Bad Request");
            }

            std::string key = line.substr( 0, pos );
            std::string value = line.substr( pos + 1 );

            // key und value entweder komplett lowercase oder uppercase machen, wegen einheitlichem handling - case sensitive 
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

HttpParser::~HttpParser()
{
    std::cout << "HttpParsing END" << std::endl;
}

// Errorcodes fuer pailed parsing: incorrect syntax 400 Bad Request