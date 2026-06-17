#include "../inc/Method.hpp"
#include "../inc/Executor.hpp"

Method::Method() {}

Method::~Method() {}

void    Method::_setResponse(Response &res,
    const std::string& statusCode,
    const std::string& reasonPhrase,
    const std::string& contentType) {
        res.setCodeAndPhrase(statusCode, reasonPhrase);
        res.setHeaders("Content-Length", std::to_string(_fileContent.size()));
        if (!_fileContent.empty()) {
            res.setHeaders("Content-Type", contentType);
            res.setBody(std::move(_fileContent));
        }
		res.build();
}

void	Method::_createAutoIndexPage(const std::string &path, Response &res,
	const HttpParser& parser)
{
    std::string& html = _fileContent;

    html += "<!DOCTYPE html>\n";
	html += "<html>\n";
	html += "<head><title>index of " + parser.getPath() + "</title></head>\n";
    html += "<body>\n";
	html += "<h1>Index of " + parser.getPath() + "</h1>\n";
    html += "<hr>\n";
    html += "<pre>\n";

    for (const auto& x : std::filesystem::directory_iterator(path)) /// filesystem issues
    {
        auto name = x.path().filename().string();    
        if (x.is_regular_file())
            html += "<a href=\"" +  name + "\">" + name + "</a>\n";
        else if (x.is_directory())
            html += "<a href=\"" +  name + "/\">" + name + "/</a>\n";
    }

    html += "</pre>\n</body>\n</html>\n";

	_setResponse(res, "200", "OK", "text/html");
}

void    Method::getMethod(const std::string &path, Response &res,
	const HttpParser& parser, const LocIndexConf* locIndexConf) // status codes 200, 402, 404
{
    std::error_code ec;
    
    if ( std::filesystem::is_regular_file(path) )
    {
        if ( !std::filesystem::exists( path, ec ) )
            throw NotFound();

        std::ifstream ifs(path, std::ios::binary); 
        if (!ifs) // permissions check
            throw Forbidden();

        _fileContent = std::string(std::istreambuf_iterator<char>(ifs),
			std::istreambuf_iterator<char>());
        _setResponse(res, "200", "OK", getFileType(path));
    }
    else if ( std::filesystem::is_directory(path) )
    {
        if (path.back() != '/')
			throw MovedPermanently(parser.getPath() + "/");
    
        if (locIndexConf) {
            for (auto& x : locIndexConf->indexFiles) // replace stack with all files in directory - indexes from location 
            {
                std::string indexPath = path + x;
                    
                if (std::filesystem::exists(indexPath, ec))
                {
                    std::ifstream ifs(indexPath); 
                    if (!ifs) // permissions check
                        continue;

                    _fileContent = std::string(std::istreambuf_iterator<char>(ifs),
						std::istreambuf_iterator<char>());
					_setResponse(res, "200", "OK", getFileType(indexPath));
                    return ;
                }
            }
            
            if (*locIndexConf->autoindex) {
                _createAutoIndexPage(path, res, parser);
                return ;
            }
        }
        throw Forbidden();
    }
    else
        throw Forbidden();
}

void    Method::deleteMethod( std::string path, Response &res) // status codes 200, 402, 404
{
    std::error_code ec;
    if ( !std::filesystem::exists( path, ec ) )
        throw NotFound();

    if (std::filesystem::remove_all( path , ec) == (unsigned long)-1)
		throw Forbidden();

	_setResponse(res, "204", "No Content", "");
}

void    Method::postMethod(
	const std::string &path, Response &res, const HttpParser& parser) // status codes 200, 402, 404
{   
    if (!std::filesystem::is_directory(path))
		throw Forbidden();

	auto it = parser.getHeaders().find("content-type");
	if (it == parser.getHeaders().end())
		throw BadRequest();

    std::string fileName = "/upload" + getTimeStamp() + getExtension(it->second);

    std::ofstream ofs(path + fileName);
    if ( !ofs )
        throw BadRequest();
    ofs << parser.getBody();

    _setResponse(res, "201", "Created", getFileType(path));
}

void    Method::runCgi(
	const std::string& path, Response &res, const HttpParser& parser) ///dynamic path form request instead of hardcoded getCgiScript function
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
		std::string gateway = "GATEWAY_INTERFACE=" + std::string("CGI/1.1");
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

        perror("execve failed");
        _exit(1);
    }
    else
    {
        close( inPipe[ 0 ] );
        close( outPipe[ 1 ] );

        char buffer[ 1024 ];
        ssize_t bytesRead;

		std::string	content;
        while ( ( bytesRead = read( outPipe[ 0 ], buffer, sizeof( buffer ))) > 0 ) {
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
