#include "../inc/Method.hpp"

Method::Method() : _isCgiFile( false ) {}

Method::~Method() {}

void    Method::_setResponse(Response &res,
    const std::string& statusCode,
    const std::string& reasonPhrase,
    const std::string& path) {
        res.setCodeAndPhrase(statusCode, reasonPhrase);
        res.setHeaders("Content-Length", std::to_string(_fileContent.size()));
        if (!_fileContent.empty()) {
            res.setHeaders("Content-Type", getFileType(path));
            res.setBody(std::move(_fileContent));
        }
}

void    Method::getMethod( std::string path, Response &res, const LocNode& location ) // status codes 200, 402, 404
{
    std::error_code ec;
    
    if ( std::filesystem::is_regular_file( path ) )
    {
        std::cout << "is a file1" << std::endl;

        if ( !( std::filesystem::exists( path, ec ) ) )
            throw NotFound();

        std::ifstream ifs(path, std::ios::binary); 
        if ( !( ifs.is_open() ) ) // permissions check
            throw NotFound();

        _fileContent = std::string(
            (std::istreambuf_iterator<char>(ifs)),
            std::istreambuf_iterator<char>());
        _setResponse(res, "200", "OK", path);
        return ;
    }
    else if ( std::filesystem::is_directory( path ) )
    {
        if ( path.back() != '/' )
        {
            // std::cout << "_root : " << _root << std::endl; 
            // std::string newStr = path.substr( _root.size() ) + "/";
            throw MovedPermanently( path );
        }
    
        if (2 <= location.locConfs.size()) {
            std::cout << location.locConfs.size() << std::endl;
            for ( auto x : dynamic_cast<LocIndexConf*>(location.locConfs[1].get())->indexFiles) // replace stack with all files in directory - indexes from location 
            {
                std::string joinedPath = path + x;
                
                std::cout << "joinedPath: " << joinedPath << std::endl;
    
                if ( std::filesystem::exists( joinedPath, ec ) )
                {
                    std::cout << "entered" << std::endl;
                    std::ifstream ifs( joinedPath ); 
                    std::cout  << joinedPath << std::endl;
    
                    if ( !( ifs.is_open() ) ) // permissions check
                        throw NotFound();

                    std::ostringstream oss;
            
                    oss << ifs.rdbuf();
    
                    _fileContent = oss.str();
    
                    _setResponse(res, "200", "OK", path);
                    // res.setBody( content );
                    // res.setCodeAndPhrase( "200", "OK" );
                    // res.setHeaders( "Content-Length", std::to_string( content.size() ) );
                    // res.setHeaders( "Content-Type", getFileType( joinedPath ) );
                    std::cout << "GET function is done" << std::endl;
    
                    return ;
                }
            }
            
            auto* locConf = dynamic_cast<LocIndexConf*>( location.locConfs[ 1 ].get() );
            if ( locConf && locConf->autoindex && *locConf->autoindex )
            {
                std::cout << "autoindex" << std::endl;
                createAutoIndex( path, res );
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
void    Method::deleteMethod( std::string path, Response &res, const LocNode& location ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    (void)location;

    std::error_code ec;
    if ( !( std::filesystem::exists( path, ec ) ) )
        throw NotFound();

    if ( std::filesystem::is_regular_file( path ) )
    {
        std::cout << "Enter delete function" << std::endl;

        std::ifstream ifs( path ); 
    
        if ( !( ifs.is_open() ) ) // permissions check
            throw NotFound();
        // delete file
        std::filesystem::remove( path );

        res.setCodeAndPhrase( "204", "No Content" );
        res.setHeaders( "Content-Length", "0" );
        
        // eventuell message in Body: File deleted successfully - dann aber andere Codes und Phrase

        return ;
    }
    else // is directory, 403 Forbidden oder wenn delete directory explizit erlaubt ist
    {
        if ( getAllowDeleteDir() == true ) // deleting directory is allowed
        {
            std::filesystem::remove_all( path );
            res.setCodeAndPhrase( "204", "No Content" );
            res.setHeaders( "Content-Length", "0" );
            
            // eventuell message in Body: Folder deleted successfully

            return ;
        }

        throw Forbidden();
    }
}

// test: printf 'POST /images HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
void    Method::postMethod( std::string path, Response &res, std::string contentBody, const LocNode& location, std::unordered_map<std::string, std::string>	headers ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    (void)location;
    
    if ( !std::filesystem::is_directory( path ) )
        throw Forbidden();

    // create file 
    std::string fileName = "/upload";
    fileName += getTimeStamp();
    // fileName += ".bin"; //use map to determine file extension
    std::string type = headers[ "content-type" ];
    // if ( !type )
        // content-type is empty
    std::string extension = getExtension( type );
    fileName += extension; //use map to determine file extension
    
    std::cout << "FileName: " << fileName << "\n" << "JoinedPath: " << path + fileName << "\n\nPosted Body:\n" << contentBody << std::endl;
    _postedFile =  path + fileName;
    std::ofstream ofs( _postedFile );
    
    if ( !ofs )
        throw BadRequest();

    ofs << contentBody;
    _setResponse(res, "201", "Created", path);
    return ;
}

void    Method::runCgi( std::string &content, bool isPost ) ///dynamic path form request instead of hardcoded getCgiScript function
{
    //std::string content;
        // runCgi( content, true ); // put CGI output to response
        
        // res.setBody(std::move(_fileContent));
        // res.setHeaders( "Content-Length", std::to_string( content.size() ) );
        // res.setHeaders( "Content-Type", getFileType( _postedFile ) );
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