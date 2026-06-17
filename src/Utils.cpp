#include <Utils.hpp>
#include "Response.hpp"

std::string setStatus(int code)
{
    static const std::unordered_map<int, std::string> reasons = {
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

    auto it = reasons.find(code);
    if (it != reasons.end())
        return it->second;
    else
        return "Unknown";
}

std::string getFileType(const std::string& path)
{
    static const std::unordered_map<std::string, std::string> map = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".text", "text/plain"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".png", "image/png"},
        {".jpeg", "image/jpeg"}
    };

    size_t pos = path.rfind(".");
    if ( pos == std::string::npos )
        return "application/octet-stream";

    std::string ext = path.substr(pos);
    for ( auto& x : ext )
        x = tolower( static_cast<unsigned char>( x ) );

    auto it = map.find(ext);
    if (it != map.end())
        return it->second;
    else
        return "application/octet-stream";
}

std::string getExtension(const std::string& contentType)
{
    static const std::unordered_map<std::string, std::string> map = {
        {"text/html", ".html"},
        {"text/css", ".css"},
        {"text/plain", ".text"},
        {"application/javascript", ".js"},
        {"application/json", ".json"},
        {"application/xml", ".xml"},
        {"image/png", ".png"},
        {"image/jpeg", ".jpeg"}
    };

    auto it = map.find(contentType);
    if (it != map.end())
        return it->second;
    else
        return ".bin";
}
