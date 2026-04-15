#include "../inc/Httpparser.hpp"

HttpParser::HttpParser( std::string request ) : _request( request )
{
    std::cout << "HttpParsing" << std::endl;
}

HttpParser::~HttpParser()
{
    std::cout << "HttpParsing" << std::endl;
}

int HttpParser::parse()
{
 
    std::cout << "Received byte:\n\n" << _request << std::endl; 
    
    return 0;
}

std::string HttpParser::getRequest()
{
    return _request;
}