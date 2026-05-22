#include "../inc/Method.hpp"

Method::Method() {}

Method::~Method() {}

std::string    Method::modifyPath( std::string uri, whichMethod whichMethod )
{
        // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";

    std::string::size_type pos = uri.find( '?' );
    std::string path = uri;
    std::string query;
    
    if ( pos != std::string::npos )
    {
        path = uri.substr( 0, pos );
        query = uri.substr( pos + 1 );
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
    
    // std::cout << "newPath == " << newPath << std::endl; 

    std::string mockLocation = "/images";
    std::string mockRoot = getRootPath();

    if ( whichMethod == METHOD_POST && getUploadEnabled() )
    {
        if( !( path.empty() ) )
            mockRoot = getUploadPath();   
        std::cout << mockRoot << std::endl;
    }
    newPath = newPath.substr( mockLocation.size() );
    
    // /DO.PNG
    if ( !( newPath.empty() ) && newPath[ 0 ] == '/' )
        newPath = newPath.substr( 1 );
    if ( mockRoot.back() != '/' )
        mockRoot += '/';
    newPath = mockRoot + newPath;

    // std::cout << "newPath:" << newPath << std::endl;
    
    std::cout << "finished modify path" << std::endl;
    return newPath;
}


void    Method::getMethod( std::string newPath, Response &res ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::vector<std::string> stack;
    stack.push_back("index1.html");
    stack.push_back("index2.html");
    stack.push_back("index3.html");

    // std::cout << "newPath:" << newPath << std::endl;

    std::error_code ec;
    if ( !( std::filesystem::exists( newPath, ec ) ) )
        throw NotFound();

    if ( std::filesystem::is_regular_file( newPath ) )
    {
        std::ifstream ifs( newPath ); 
    
        if ( !( ifs.is_open() ) ) // permissions check
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
        res.setHeaders( "Content-Type", getFileType( newPath ) );
        return ;
    }
    else //    if ( std::filesystem::is_directory( newPath ) )
    {
        if ( newPath.back() != '/' )
        {
            std::string newStr = newPath + "/";
            throw MovedPermanently( newStr );
        }
        for ( auto x : stack ) // replace stack with all files in directory - indexes from location 
        {
            std::string joinedPath = newPath + x;
            if ( std::filesystem::exists( joinedPath, ec ) )
            {
                std::ifstream ifs( joinedPath ); 
                std::cout  << joinedPath << std::endl;

                if ( !( ifs.is_open() ) ) // permissions check
                    throw NotFound();
                std::string line;
                std::string content;
                while ( getline( ifs, line ) )
                {
                    content += line;
                    std::cout  << "here" << std::endl;
                }
                res.setBody( content );
                res.setCodeAndPhrase( "200", "OK" );
                res.setHeaders( "Content-Length", std::to_string( content.size() ) );
                res.setHeaders( "Content-Type", getFileType( joinedPath ) );
                std::cout << "GET function is done" << std::endl;

                return ;
            }
        }

        if ( autoIndexActive() ) // not implemented yet   
        {
            createAutoIndex( newPath, res ); // not implemented yet
            return ;
        }
        else
            throw NotFound();
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

void    Method::postMethod( std::string newPath, Response &res ) // status Codes 200/201, 400, 413
{
    (void)newPath;
    (void)res;
}

bool    getUploadEnabled()
{
    return true;
}

std::string getUploadPath()
{
    return "/uploads/";
}

std::string getRootPath()
{
    return "./servers/server1/uploads/data";
}


void    Method::deleteMethod( std::string newPath, Response &res ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::cout << "newPath:" << newPath << std::endl;


    std::error_code ec;
    if ( !( std::filesystem::exists( newPath, ec ) ) )
        throw NotFound();

    if ( std::filesystem::is_regular_file( newPath ) )
    {
        std::cout << "Enter delete function" << std::endl;

        std::ifstream ifs( newPath ); 
    
        if ( !( ifs.is_open() ) ) // permissions check
            throw NotFound();
        // delete file
        std::filesystem::remove( newPath );

        res.setCodeAndPhrase( "204", "No Content" );
        res.setHeaders( "Content-Length", "0" );
        return ;
    }
    else // is directory, 403 Forbidden oder wenn delete directory explizit erlaubt ist
    {
        if ( getAllowDeleteDir() == true ) // deleting directory is allowed
        {
            std::filesystem::remove_all( newPath );
            res.setCodeAndPhrase( "204", "No Content" );
            res.setHeaders( "Content-Length", "0" );
            return ;
        }

        throw Forbidden();
    }
}

void    Method::postMethod( std::string newPath, Response &res ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::cout << "newPath:" << newPath << std::endl;


    std::error_code ec;
    if ( ( std::filesystem::exists( newPath, ec ) ) ) // wenn file existiert muessen wir checken, ob wir ueberschreiben duerfen? sonst exception?
        throw NotFound();

    if ( std::filesystem::is_regular_file( newPath ) )
    {
        std::cout << "Enter delete function" << std::endl;

        std::ifstream ifs( newPath ); 
    
        if ( !( ifs.is_open() ) ) // permissions check
            throw NotFound();

        // put content

        res.setCodeAndPhrase( "204", "No Content" );
        res.setHeaders( "Content-Length", "0" );
        return ;
    }
    else // is directory, 403 Forbidden oder wenn delete directory explizit erlaubt ist
    {
        // create file 

        res.setCodeAndPhrase( "200", "OK" );
        res.setHeaders( "Content-Length", "0" );
        return ;

        throw Forbidden();
    }
}


// test: printf 'DELETE /images HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490

bool getAllowDeleteDir()
{
    return false;
}