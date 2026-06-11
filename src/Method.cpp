#include "../inc/Method.hpp"

Method::Method() : _isCgiFile( false ) {}

Method::~Method() {}

std::string    Method::joinRootAndPath(
    std::string newPath, whichMethod whichMethod, const IWebservModule::LocNode& location)
{

    // std::string mockLocation = "/images";
    // std::string root = getRootPath();
    if (!location.locConfs.empty()) {
        const auto* locConf =\
        dynamic_cast<LocCoreConf*>(location.locConfs[0].get());
        _root = locConf->root.empty() ? getRootPath() : locConf->root;
    }
    else _root = getRootPath();

    if ( whichMethod == METHOD_POST )
    {
        if ( !getUploadEnabled() )
            throw Forbidden();
        if( !( _path.empty() ) )
            _root = getUploadPath().empty() ? _root : getUploadPath();
        std::cout << "Upload Path: " << _root << std::endl;
    }

    // newPath = mockLocation + newPath;
    // newPath = newPath.substr( mockLocation.size() );
    
    // /DO.PNG
    
    std::cout << "root:" << _root << "\nnewPath:" << newPath << std::endl;
    newPath = _root + newPath; // join root + uri 

    
    std::cout << "finished modify path" << std::endl;
    return newPath;
}

std::string    Method::normalizePath(std::string uri)
{
        // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    
    // Normalizing
    std::cout << "uri: " << uri << std::endl;

    std::string::size_type pos = uri.find( '?' );
    
    _path = uri;
    
    bool endsWithSlash = !_path.empty() && _path.back() == '/';

    if ( pos != std::string::npos )
    {
        _path = uri.substr( 0, pos );
        _query = uri.substr( pos + 1 );
    }

    checkCgiExtension();

    for ( size_t i = 0; i < _path.size(); ++i )
    {
        if ( _path[ i ] == '%' && i + 2 < _path.size() )
        {
            std::string hex = _path.substr( i + 1, 2 );
            char c = static_cast<char>( std::strtol( hex.c_str(), 0, 16 ) );
            _path.replace( i, 3, 1, c );
        }
    }
    // std::cout << "_Path After: " << _path << std::endl;
    // std::cout << "Query: " << _query << std::endl;

    std::vector<std::string> wholePath;
    std::istringstream iss( _path );
    std::string partStr;

    while ( getline( iss, partStr, '/' ) )
    {
        if ( partStr.empty() || partStr == "." ) // "." - dieses Verzeichnis
            continue ;
        if ( partStr == ".." ) // Traversal-Check 
        {
            if ( wholePath.empty() )
                throw BadRequest();
                
            wholePath.pop_back(); // one directory out  
        }
        else
            wholePath.push_back( partStr );
    }

    std::string newPath;
    for ( size_t i = 0; i < wholePath.size(); ++i )
    {
        newPath = "/" + newPath;
        newPath += wholePath[ i ];
    }
    std::cout << "new Path: " << newPath << std::endl;
    
    if ( endsWithSlash )
        newPath += '/';
    return newPath;
}


void    Method::getMethod( std::string newPath, Response &res, const IWebservModule::LocNode& location ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::error_code ec;
    // std::vector<std::string> stack;
    // stack.push_back("index1.html");
    // stack.push_back("index2.html");
    // stack.push_back("index3.html");
    // stack.push_back("index.html");

    std::cout << "newPath:" << newPath << std::endl;

    //  printf 'GET /servers/server1/cgi/test.py HTTP/1.1\r\n\r\n' | nc 127.0.0.2 3490

    
    if ( std::filesystem::is_regular_file( newPath ) )
    {
        std::cout << "is a file1" << std::endl;

        if ( !( std::filesystem::exists( newPath, ec ) ) )
        {
            std::cout << "Geht hier raus" << std::endl;
            throw NotFound();
        }

        std::string content;

        if ( _isCgiFile ) // || getIsCgiLocation()
            runCgi( content, false ); // put CGI output to response
        else
        {
            std::cout << "is a file2" << std::endl;

            std::ifstream ifs( newPath ); 
        
            if ( !( ifs.is_open() ) ) // permissions check
                throw NotFound();
            
            std::ostringstream oss;
            
            oss << ifs.rdbuf();

            content = oss.str();
        }

        std::cout << "is a file3" << std::endl;

        res.setBody( content );
        res.setCodeAndPhrase( "200", "OK" );
        res.setHeaders( "Content-Length", std::to_string( content.size() ) );
        res.setHeaders( "Content-Type", getFileType( newPath ) );
        return ;
    }
    else if ( std::filesystem::is_directory( newPath ) )
    {
        std::cout << "newP: " << newPath << std::endl;
        if ( newPath.back() != '/' )
        {
            std::cout << "_root : " << _root << std::endl; 
            std::string newStr = newPath.substr( _root.size() ) + "/";
            throw MovedPermanently( newStr );
        }
    
        if (2 <= location.locConfs.size()) {
            std::cout << location.locConfs.size() << std::endl;
            for ( auto x : dynamic_cast<LocIndexConf*>(location.locConfs[1].get())->indexFiles) // replace stack with all files in directory - indexes from location 
            {
                std::string joinedPath = newPath + x;
                
                std::cout << "joinedPath: " << joinedPath << std::endl;
    
                if ( std::filesystem::exists( joinedPath, ec ) )
                {
                    std::cout << "entered" << std::endl;
                    std::ifstream ifs( joinedPath ); 
                    std::cout  << joinedPath << std::endl;
    
                    if ( !( ifs.is_open() ) ) // permissions check
                        throw NotFound();
                    std::string content;
                    std::ostringstream oss;
            
                    oss << ifs.rdbuf();
    
                    content = oss.str();
    
                    res.setBody( content );
                    res.setCodeAndPhrase( "200", "OK" );
                    res.setHeaders( "Content-Length", std::to_string( content.size() ) );
                    res.setHeaders( "Content-Type", getFileType( joinedPath ) );
                    std::cout << "GET function is done" << std::endl;
    
                    return ;
                }
            }
    
            if (dynamic_cast<LocIndexConf*>(location.locConfs[1].get())->autoindex)
            {
                std::cout << "autoindex" << std::endl;
                createAutoIndex( newPath, res ); // not implemented yet
                return ;
            }
        }
        throw NotFound();
    }
    else
        throw NotFound();
}
// /servers/server1/uplodas/data/index1.html
// /servers/server1/uploads/data/index1.html

// test: printf 'DELETE /images HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
void    Method::deleteMethod( std::string newPath, Response &res, const IWebservModule::LocNode& location ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    (void)location;

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
        
        // eventuell message in Body: File deleted successfully - dann aber andere Codes und Phrase

        return ;
    }
    else // is directory, 403 Forbidden oder wenn delete directory explizit erlaubt ist
    {
        if ( getAllowDeleteDir() == true ) // deleting directory is allowed
        {
            std::filesystem::remove_all( newPath );
            res.setCodeAndPhrase( "204", "No Content" );
            res.setHeaders( "Content-Length", "0" );
            
            // eventuell message in Body: Folder deleted successfully

            return ;
        }

        throw Forbidden();
    }
}

// test: printf 'POST /images HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
void    Method::postMethod( std::string newPath, Response &res, std::string contentBody, const IWebservModule::LocNode& location ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    (void)location;
        
    if ( std::filesystem::is_regular_file( newPath ) )
    {
        if ( !getAllowedToOverwrite() )
            throw Forbidden();
        else
        {
            _postedFile = newPath;
            std::ofstream ofs( _postedFile, std::ios::binary | std::ios::trunc );
            if ( !ofs )
                throw BadRequest();

            // put content
            ofs << contentBody;
            ofs.close();

            res.setCodeAndPhrase( "200", "OK" );

            if ( getIsCgiLocation() ) //_isCgiFile fuer .py endungen 
            {
                std::string content;
                runCgi( content, true ); // put CGI output to response

                res.setBody( content );
                res.setHeaders( "Content-Length", std::to_string( content.size() ) );
                res.setHeaders( "Content-Type", getFileType( _postedFile ) );
            }
            else
                res.setHeaders( "Content-Length", "0" );
            
            return ;
        }        
    }
    else
    {
        if ( newPath.back() != '/' )
        {
            size_t pos = newPath.find_last_of( '/' );
            if ( pos != std::string::npos )
                newPath.erase( pos + 1 );
            
        }
        // create file 
        std::string fileName = "upload";
        fileName += getTimeStamp();
        fileName += ".bin";
        
        std::cout << "FileName: " << fileName << "\n" << "JoinedPath: " << newPath + fileName << "\n\nPosted Body:\n" << contentBody << std::endl;
        _postedFile =  newPath + fileName;
        std::ofstream ofs( _postedFile );
        
        if ( !ofs )
            throw BadRequest();

        ofs << contentBody;

        ofs.close();
        
        res.setCodeAndPhrase( "201", "Created" );
        
        if ( getIsCgiLocation() )
        {
            std::string content;
            runCgi( content, true ); // put CGI output to response
            
            res.setBody( content );
            res.setHeaders( "Content-Length", std::to_string( content.size() ) );
            res.setHeaders( "Content-Type", getFileType( _postedFile ) );
        }
        else
            res.setHeaders( "Content-Length", "0" );
            
        return ;
    }
}

void    Method::runCgi( std::string &content, bool isPost ) ///dynamic path form request instead of hardcoded getCgiScript function
{
    int inPipe[2];
    int outPipe[2];

    pipe( inPipe );
    pipe( outPipe );

    pid_t pid = fork();

    if ( pid == 0 )
    {
        std::string cgi = getCgiPath();
        std::string script = getScript();

        if ( isPost )
            script = _postedFile;

        _query += "QUERY_STRING=" + _query;
        char *envp[] = { ( char *)_query.c_str(), NULL };
        std::cout << "Evnp: " << envp[0] << std::endl;
        char *argv[] = { ( char *)cgi.c_str(), ( char *)script.c_str(), NULL };

        dup2( inPipe[ 0 ], STDIN_FILENO );
        dup2( outPipe[ 1 ], STDOUT_FILENO );
        close( inPipe[ 1 ] );
        close( outPipe[ 0 ] );

        // std::cout << "cgi = " << cgi << std::endl;
        // std::cout << "script = " << script << std::endl;
        execve( cgi.c_str(), argv, envp ); // returned direkt aus function?

        perror( "execve failed" );
        _exit( 1 );
    }
    else
    {
        close( inPipe[ 0 ] );
        close( outPipe[ 1 ] );

        char buffer[ 1024 ];
        ssize_t bytesRead;

        while ( ( bytesRead = read( outPipe[ 0 ], buffer, sizeof( buffer ))) > 0 )
        {
            content.append( buffer, bytesRead );
        }

        close( outPipe[ 0 ] );

        int status;
        waitpid( pid, &status, 0 );
    }
}

bool    getAllowDeleteDir()
{
    return false;
}

bool    getAllowedToOverwrite()
{
    return true;
}

std::string getTimeStamp()
{
    return std::to_string( std::time( 0 ) );
}

std::string Method::getCgiPath()
{
    return "/usr/bin/python3";
}

std::string Method::getScript()
{
    return "./servers/server1/cgi/test.py";
}

bool    getIsCgiLocation()
{
    return false;
}

bool    getUploadEnabled()
{
    return true;
}

std::string getUploadPath()
{
    return "./servers/server1/uploads";
}

std::string getRootPath()
{
    return "./servers/server1/";
}

bool autoIndexActive() // still to implement: return bool for autoindex
{
    return true;
}

void    Method::checkCgiExtension()
{
    size_t size = _path.size(); 

    if ( size >= 3 && _path.compare( size - 3, 3, ".py" ) == 0 )
        _isCgiFile = true;
}