#include "../inc/httpparser.hpp"

HttpParsing::HttpParser( std::string request ) : _result(request )
{
    std::cout << "HttpParsing" << std::endl;
}

HttpParsing::~HttpParser() : _result()
{
    std::cout << "HttpParsing" << std::endl;
}

int HttpParsing::parse()
{

}

std::string HttpParsing::getRequest()
{
    return _request;
}