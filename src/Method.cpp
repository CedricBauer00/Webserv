#include "../inc/Method.hpp"
#include "../inc/HttpParser.hpp"
Method::Method() {}

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

void    Method::deleteMethod( std::string path, Response &res) // status codes 200, 402, 404
{
    std::error_code ec;
    if ( !( std::filesystem::exists( path, ec ) ) )
        throw NotFound();

    if (std::filesystem::remove_all( path , ec) == (unsigned long)-1)
		throw Forbidden();

	_setResponse(res, "204", "No Content", path);
}

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

void    Method::runCgi(const std::string& path, Response &res, const HttpParser& parser) ///dynamic path form request instead of hardcoded getCgiScript function
{
    if (!std::filesystem::is_regular_file(path))
		throw NotFound();
    int inPipe[2];
    int outPipe[2];

    pipe( inPipe );
    pipe( outPipe );

    pid_t pid = fork();

    if ( pid == 0 )
    {
        std::string cgi; //= getCgiPath();
        std::string script; //= getScript();

		std::string gateway = "GATEWAY_INTERFACE=" + parser.getQuery();
        std::string query = "QUERY_STRING=" + parser.getQuery();
		std::string raddr = "REMOTE_ADDR=" + std::string("203.0.113.42");
		std::string reqMethod = "REQUEST_METHOD=" + parser.getMethodStr();
		std::string scriptName = "SCRIPT_NAME=" + parser.getPath();
		std::string srvName = "SERVER_NAME=" + parser.getHostName();
		std::string srvPort = "SERVER_PORT=" + parser.getHostPort();
		std::string srvProtocol = "SERVER_PROTOCOL=" + parser.getHttp();



        char *envp[] = {(char *)gateway.c_str(),
			(char *)query.c_str(),
			(char *)raddr.c_str(),
			(char *)reqMethod.c_str(),
			(char *)scriptName.c_str(),
			(char *)srvName.c_str(),
			(char *)srvPort.c_str(),
			(char *)srvProtocol.c_str(),
			NULL };
        std::cout << "Evnp: " << envp[0] << ", "
		<< envp[1] << ", "
		<< envp[2] << ", "
		<< envp[3] << ", "
		<< envp[4] << ", "
		<< envp[5] << ", "
		<< envp[6] << ", "
		<< envp[7] << std::endl;

        dup2( inPipe[ 0 ], STDIN_FILENO );
        dup2( outPipe[ 1 ], STDOUT_FILENO );
        close( inPipe[ 1 ] );
        close( outPipe[ 0 ] );

        char *argv[] = {(char *)path.c_str(), NULL};
        execve(path.c_str(), argv, envp ); // returned direkt aus function?

        perror( "execve failed" );
        _exit( 1 );
    }
    else
    {
        close( inPipe[ 0 ] );
        close( outPipe[ 1 ] );

        char buffer[ 1024 ];
        ssize_t bytesRead;

		std::string	content;
        while ( ( bytesRead = read( outPipe[ 0 ], buffer, sizeof( buffer ))) > 0 )
        {
            content.append( buffer, bytesRead );
        }

        close( outPipe[ 0 ] );

        int status;
        waitpid( pid, &status, 0 );
		res.setBody(std::move(content));
    }
}

std::string getTimeStamp()
{
    return std::to_string( std::time( 0 ) );
}
