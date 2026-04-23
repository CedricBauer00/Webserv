#include "../inc/HttpParser.hpp"

HttpParser::HttpParser() : _startLine(), _headers(), _body()
{
    std::cout << "HttpParsing" << std::endl;
}

void    HttpParser::setHeaders( std::string request )
{
    std::istringstream iss( request );
    std::string line;

    std::cout << GREEN << "request = " << request << RESET << std::endl;

    while ( std::getline( iss, line ) )
    {
        if ( !line.empty() && line.back() == 'r' )
            line.pop_back();
        if ( _startLine.empty() )
            setStartLine( line );
        else
        {
            
        }
    }
}

void    HttpParser::setStartLine( std::string line )
{
    std::istringstream  iss( line );
    std::string         token;
    
    while ( iss >> token )
        _startLine.push_back( token );
}

HttpParser::~HttpParser()
{
    std::cout << "HttpParsing" << std::endl;
}