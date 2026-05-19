#include "../inc/Method.hpp"

Method::Method() {}

Method::~Method() {}

void    Method::getMethod() // status codes 200, 402, 404
{
    std::string mockUri = "/images/cat%20pics/../dog.png?size=large&debug=1";

    std::string::size_type pos = mockUri.find( '?' );
    if ( pos != std::string::npos )
    {
        std::string path = mockUri.substr( 0, pos );
        std::string query = mockUri.substr( pos + 1 );
    }

    for ( size_t i = 0; i < path.size(); ++i )
    {
        if ( path[ i ] == '%' && i + 2 < path.size() )
        {
            std::string hex = path.substr( pos + 1, 2 );
            char c = static_cast<char>( std::strtol( hex.c_str(), 0, 16 ) ); // 
            path.replace( i, 3, 1, c );
        }
    }

    std::vector<std::string> wholePath;
    std::istringstream iss(path);
    std::string partStr;
    while ( getline( iss, partStr, '/' ) )
    {
        if ( partStr.empty() || partStr == "." ) // "." heisst dieses Verzeichnis
            continue ;
        if ( partStr == ".." )
        {
            if ( wholePath.empty() )
                throw BadRequest();
            wholePath.pop_back( ); // one directory out  
        }
        else
        {
            wholePath.push_back( partStr );
        }
    }

    std::string newPath;
    newPath = "/";
    for ( size_t i = 0; i < wholePath.size(); ++i )
    {
        
    }
    // 1) Method permissions pruefen passiert in execution func - check ob syntax korrekt?
    //  wenn ein body bei GET method - ignoreiren

    // 2) URI - Filesystem-path ( join root + uri, Normalisierung, Traversal-Check )
    // Exists - nein? - 404
    
    // 3) is a directory?
        // ja -> Indexsuche 
        // if found - follow file path
        // if not - check autoindex
        // otherwise 403
    
    // 4) Is a file? 
        // yes - read file
        // get content-type
        // set content-length
        // build response
    // 5) I/O-Fehler - 500

}

void    Method::postMethod() // status Codes 200/201, 400, 413
{

}

void    Method::deleteMethod() // status Codes 200/204, 403, 404
{
    
}