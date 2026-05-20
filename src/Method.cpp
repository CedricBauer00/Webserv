#include "../inc/Method.hpp"

Method::Method() {}

Method::~Method() {}

void    Method::getMethod( Response& res) // status codes 200, 402, 404
{
    std::string mockUri = "/images/cat%20pics/../dog.png?size=large&debug=1";

    std::string::size_type pos = mockUri.find( '?' );
    std::string path;
    std::string query;
    
    if ( pos != std::string::npos )
    {
        path = mockUri.substr( 0, pos );
        query = mockUri.substr( pos + 1 );
    }

    for ( size_t i = 0; i < path.size(); ++i )
    {
        if ( path[ i ] == '%' && i + 2 < path.size() )
        {
            std::string hex = path.substr( i + 1, 2 );
            char c = static_cast<char>( std::strtol( hex.c_str(), 0, 16 ) ); // 
            path.replace( i, 3, 1, c );
        }
    }

    std::vector<std::string> wholePath;
    std::istringstream iss(path);
    std::string partStr;
    // int i = 0;
    while ( getline( iss, partStr, '/' ) )
    {
        std::cout << "partStr:" << partStr << std::endl;

        if ( partStr.empty() || partStr == "." ) // "." heisst dieses Verzeichnis
            continue ;
        if ( partStr == ".." )
        {
            if ( wholePath.empty() )
                throw BadRequest();
                
            wholePath.pop_back(); // one directory out  
        }
        else
            wholePath.push_back( partStr );
        // std::cout << "wholePath:" << wholePath[ i ] << std::endl;
        // i++;
    }

    std::string newPath;
    newPath = "/";
    for ( size_t i = 0; i < wholePath.size(); ++i )
    {
        newPath += wholePath[ i ];
        if ( i + 1 < wholePath.size() )
            newPath += "/";
    }
    
    std::cout << "newPath == " << newPath << std::endl; 

    std::string mockLocation = "/images";
    std::string mockRoot = "/root";
    std::vector<std::string> stack;
    stack.push_back("index1.html");
    stack.push_back("index2.html");
    stack.push_back("index3.html");

    newPath = newPath.substr( mockLocation.size() );
    
    // /DO.PNG
    if ( !( newPath.empty() ) && newPath[ 0 ] == '/' )
        newPath = newPath.substr( 1 );
    if ( mockRoot.back() != '/' )
        mockRoot += '/';
    newPath = mockRoot + newPath;

    std::cout << "newPath:" << newPath << std::endl;

    if ( !( std::filesystem::exists( newPath ) ) )
        throw NotFound();

    if ( std::filesystem::is_directory( newPath ) )
    {
        if ( newPath.back() != '/' )
        {
            std::string newStr = newPath + "/";
            throw MovedPermanently( newStr );
        }
        for ( auto x : stack )
        {
            std::string joinedPath = newPath + x;
            if ( std::filesystem::exists( joinedPath ) )
            {
                std::ifstream ifs( newPath ); 
                if ( !( ifs.is_open() ) ) 
                    throw NotFound();
                std::string line;
                std::string content;
                while ( getline( ifs, line ) )
                {
                    content += line;
                }
                res.setBody( content );
                res.setCodeAndPhrase( "200", "OK" );
                res.setHeaders( "Content-Length", std::to_string( content.size() ) );
                res.setHeaders( "Content-Type", getFileType( joinedPath ) );
                return ;
            }
        }
        if ( autoIndexActive() )   
            createAutoIndex();
        else
            throw NotFound();
    }
    // if ( std::filesystem::is_regular_file( newPath ) )
    // {
    //     if ( std::filesystem::permissions())
    // }
    // else


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

void    Method::postMethod( Response& res ) // status Codes 200/201, 400, 413
{

}

void    Method::deleteMethod( Response& res ) // status Codes 200/204, 403, 404
{
    
}

int autoIndexActive()
{

    return 0;
}

void    createAutoIndex() {}