#include <Utils.hpp>
#include "Response.hpp"

std::string setStatus(int code)
{
    static const std::map<int, std::string> reasons = {
        {200, "OK"},
        {201, "Created"},
        {204, "No Content"},
        {301, "Moved Permanently"},
        {308, "Permanent Redirect"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"},
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"},
        {423, "Locked"},
        {424, "Failed Dependency"},
        {425, "Too Early"},
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"},
        {508, "Loop Detected"},
        {510, "Not Extended"},
        {511, "Network Authentication Required"}
    };

    if (reasons.count(code))
        return reasons.at(code);
    else
        return "Unknown";
}

std::string getFileType( std::string path )
{
    size_t pos = path.rfind( "." );
    if ( pos == std::string::npos )
        return "application/octet-stream";
    
    std::string type = path.substr( pos );

    for ( auto& x : type )
        x = tolower( static_cast<unsigned char>( x ) );

    if ( type == ".html" )
        return "text/html";
    else if ( type == ".css" )
        return "text/css";
    else if ( type == ".text" )
        return "text/plain";
    else if ( type == ".js" )
        return "application/javascript";
    else if ( type == ".json" )
        return "application/json";
    else if ( type == ".xml" )
        return "application/xml";
    else if ( type == ".png" )
        return "image/png";
    else if ( type == ".jpeg" )
        return "image/jpeg";
    else
        return "application/octet-stream";
}

void createAutoIndex( std::string mockUri, Response &res )
{
    std::vector<std::string> all;
    std::string buffer;

    buffer += "<!DOCTYPE html>\n";
    buffer += "<html>\n";
    buffer += "<head>\n";
    buffer += "<title>index of " + mockUri + "/<title>\n";
    buffer += "<hr>\n";
    buffer += "<pre>\n";

    for ( auto x : std::filesystem::directory_iterator( mockUri ) ) /// filesystem issues
    {
        std::filesystem::path path = x.path();    
        if ( std::filesystem::is_regular_file( path ) )
            buffer += "<a href=\"" + mockUri +  path.filename().string() + "\">" + path.filename().string() + "</a>\n";
        else if ( std::filesystem::is_directory( path ) )
            buffer += "<a href=\"" + mockUri +  path.filename().string() + "/\">" + path.filename().string() + "</a>\n";
    }

    buffer += "</pre>\n";
    buffer += "</hr>\n";
    buffer += "</body>\n";
    buffer += "</html>\n";

    res.setBody( buffer );
    res.setCodeAndPhrase( "200", "OK" );
    res.setHeaders( "Content-Length", std::to_string( buffer.size() ) );
    res.setHeaders( "Content-Type", "txt/html" );
    res.build();
}