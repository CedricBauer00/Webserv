#include "../inc/HttpParser.hpp"

HttpParser::HttpParser()
{
    std::cout << "HttpParsing" << std::endl;
}

HttpParser::~HttpParser()
{
    std::cout << "HttpParsing" << std::endl;
}

void    HttpParser::setHeaders( std::string request )
{
    std::cout << GREEN << "request = " << request << RESET << std::endl;
    // for ( int i = 0; i < _request.size(); ++i )
    // {
    //     if (  )
    //     {

    //     }
    //     else
    //     {

    //     }
    // }
}
