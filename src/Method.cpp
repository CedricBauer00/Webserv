#include "../inc/Method.hpp"
#include "../inc/Executor.hpp"

Method::Method() {}

Method::~Method() {}

void	Method::_createAutoIndexPage(const std::string &path, Response &res,
	const AHttpParser& parser)
{
    std::string html;

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

    res.build(std::move(html), {
		{"Content-Type", "text/html"},
		{"Content-Length", std::to_string(html.size())}},
		"200", statusCodeToReasonPhrase.at(200));
}

bool    Method::getMethod(const std::string &path, Response &res,
	AHttpParser& parser, const LocIndexConf* locIndexConf) // status codes 200, 402, 404
{
    std::error_code ec;
    if ( !std::filesystem::exists( path, ec ) )
            throw NotFound();

    if ( std::filesystem::is_regular_file(path) ) {
		if (_ranCGI(path, res, parser))
			return true;
        std::ifstream ifs(path, std::ios::binary); 
        if (!ifs) // permissions check
            throw Forbidden();

        _fileContent = std::string(std::istreambuf_iterator<char>(ifs),
			std::istreambuf_iterator<char>());
		res.build(std::move(_fileContent), {
			{"Content-Type", getFileType(path)},
			{"Content-Length", std::to_string(_fileContent.size())}},
			"200", statusCodeToReasonPhrase.at(200));
		return true;
    }
    else if ( std::filesystem::is_directory(path) ) {
        if (path.back() != '/')
			throw PermanentRedirect({{"Location", parser.getPath() + "/"}});
    
        if (locIndexConf) {
            for (auto& x : locIndexConf->indexFiles) {
                if (std::filesystem::exists(path + x, ec)) {
					parser.setRedirectPath(parser.getPath() + x);
					return false;
                }
            }
            
            if (*locIndexConf->autoindex) {
                _createAutoIndexPage(path, res, parser);
                return true;
            }
        }
        throw Forbidden();
    }
    else
        throw Forbidden();
}

bool    Method::deleteMethod(
	std::string path, Response &res, const AHttpParser& parser) // status codes 200, 402, 404
{
	if (_ranCGI(path, res, parser))
		return true;

    std::error_code ec;
    if ( !std::filesystem::exists( path, ec ) )
        throw NotFound();

    if (std::filesystem::remove_all( path , ec) == (unsigned long)-1)
		throw Forbidden();

	res.build("204", statusCodeToReasonPhrase.at(204));
	return true;
}

bool    Method::postMethod(
	const std::string &path, Response &res, const AHttpParser& parser) // status codes 200, 402, 404
{
	if (_ranCGI(path, res, parser))
		return true;

    if (!std::filesystem::is_directory(path))
		throw Forbidden();

	auto it = parser.getHeaders().find("content-type");
	if (it == parser.getHeaders().end())
		throw BadRequest();

    std::string fileName = "/upload" + getTimeStamp() + getExtension(it->second);

    std::ofstream ofs(path + fileName);
    if ( !ofs )
        throw Forbidden();
    ofs << parser.getBody();

	res.build(std::move(_fileContent), {
		{"Location", parser.getPath() + fileName}},
		"201", statusCodeToReasonPhrase.at(201));
	return true;
}

bool    Method::_ranCGI(
	const std::string &path, Response &res, const AHttpParser& parser)
{
	const std::string&	reqPath = parser.getPath();
	std::size_t			len = reqPath.length();

	if (reqPath.compare(0, 5, "/cgi/") != 0)
		return false;
 
	if (reqPath.compare(len - 3, 3, ".sh") == 0
	|| reqPath.compare(len - 3, 3, ".py") == 0 ) {
		_runCgi(path, res, parser);
		return true;
	}
	throw NotFound();
}


std::vector<std::string>	Method::_getCGIEnv(const AHttpParser& parser)
{
	static auto normalize = [](const std::string& str) {
		std::string result;

		for (char c : str) {
			if (c == '-')
				result += '_';
			else
				result += std::toupper(static_cast<unsigned char>(c));
		}
		return result;
	};

	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=" + parser.getMethodStr());
    env.push_back("QUERY_STRING=" + parser.getQuery());
	env.push_back("SCRIPT_NAME=" + parser.getPath());
	env.push_back("PATH_INFO=");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_NAME=" + parser.getHostName());
	env.push_back("SERVER_PORT=" + parser.getHostPort());
	env.push_back("SERVER_PROTOCOL=" + parser.getHttp());
	env.push_back("REMOTE_ADDR=" + std::string("203.0.113.42"));

	auto& headers = parser.getHeaders();
	for (auto& header : headers) {
		if (header.first == "content-length" || header.first == "content-type")
			env.push_back(normalize(header.first) + "=" + header.second);
		else
			env.push_back("HTTP_" + normalize(header.first) + "=" + header.second);
	}
	if (!parser.getBody().empty() && headers.find("content-length") == headers.end())
		env.push_back("CONTENT_LENGTH=" + std::to_string(parser.getBody().size()));

	return env;
}

void    Method::_runCgi(
	const std::string& path, Response &res, const AHttpParser& parser) ///dynamic path form request instead of hardcoded getCgiScript function
{
	pid_t	pid;
    int 	inPipe[2];
    int 	outPipe[2];

    if (pipe(inPipe) == -1)
        throw InternalServerError();
    if (pipe(outPipe) == -1) {
        close(inPipe[0]); close(inPipe[1]);
        throw InternalServerError();
    }

    pid = fork();
	if (pid == -1) {
        close(inPipe[0]); close(inPipe[1]); close(outPipe[0]); close(outPipe[1]);
		perror("fork");
		throw InternalServerError();
	}

    if (pid == 0)
    {
		std::vector<std::string> env = _getCGIEnv(parser);

        std::vector<char*> envp;
		for (auto &e : env)
			envp.push_back(e.data());
		envp.push_back(NULL);

		std::filesystem::path p(path);
		if (chdir(p.parent_path().c_str()) == -1) {
			perror("chdir");
			_exit(1);
		}

		close( inPipe[ 1 ] );
        close( outPipe[ 0 ] );
        dup2( inPipe[ 0 ], STDIN_FILENO );
        dup2( outPipe[ 1 ], STDOUT_FILENO );
        close( inPipe[ 0 ] );
        close( outPipe[ 1 ] );

        char *argv[] = {(char *)p.filename().c_str(), NULL};
        execve(p.filename().c_str(), argv, envp.data() ); // returned direkt aus function?

        perror("execve failed");
        _exit(1);
    }
    else
    {
        int status;

		close( inPipe[ 0 ] );
        close( outPipe[ 1 ] );
		if (write(inPipe[1], parser.getBody().c_str(), parser.getBody().size()) == -1) {
			close(inPipe[1]); close(outPipe[0]);
            waitpid(pid, &status, 0);
			throw InternalServerError();
		}
		close(inPipe[ 1 ]);

        char buffer[ 1024 ];
        ssize_t bytesRead;

		std::string	content;
        while ( ( bytesRead = read( outPipe[ 0 ], buffer, sizeof( buffer ))) > 0 ) {
            content.append( buffer, bytesRead );
        }

        close( outPipe[ 0 ] );

        waitpid( pid, &status, 0 );
		if (WIFEXITED(status)) {
			int code = WEXITSTATUS(status);
			if (code)
				throw InternalServerError();
		}
		_parseCGIResponse(content, res);
    }
}

void	Method::_parseCGIResponse(const std::string& cgiRes, Response &htmlRes) {
	bool	hasContentType{false};
    bool    hasLocation{false};
	bool	hasStatus{false};

	std::string::size_type start = 0;
    std::string::size_type pos;

    while ((pos = cgiRes.find("\r\n", start)) != std::string::npos)
    {
        std::string_view line(cgiRes.data() + start, pos - start);
        if (line.empty())
        {
			if (!hasContentType && !hasLocation)
				throw BadGateway();
            if (!hasStatus)
				htmlRes.setCodeAndPhrase("200", "OK");
            htmlRes.setBody(cgiRes.substr(pos + 2));
			htmlRes.build();
            return;
        }

		std::size_t colon = line.find(':');
		if (colon == std::string_view::npos || colon == 0)
			throw BadGateway();
		if (line[colon - 1] == ' ')
			throw BadGateway();

		std::string key(line.substr(0, colon));
		std::string value(line.substr(colon + 1));

		if (key == "Status" && !hasStatus) {
			std::string::size_type spPos = value.find(' ');
			if (spPos == std::string::npos)
				throw BadGateway();
			std::string statusCode = value.substr(0, spPos);
			if (!isAllDigits(statusCode))
				throw BadGateway();
			std::string reasonPhrase = value.substr(spPos + 1);
			if (!hasSingleSpacesOnly(reasonPhrase))
				throw BadGateway();
			htmlRes.setCodeAndPhrase(std::move(statusCode),
				std::move(reasonPhrase));
			hasStatus = true;
		}
		else if (key == "Content-Type" && !hasContentType)
		{
			if (hasLocation)
				throw BadGateway();
			htmlRes.setHeaders(key, value);
			hasContentType = true;
		}
		else if (key == "Location" && !hasLocation)
		{
			if (hasContentType)
				throw BadGateway();
			htmlRes.setHeaders(key, value);
			hasLocation = true;
		}
		else
		{
			htmlRes.setHeaders(key, value);
		}
        start = pos + 2;
    }
	throw BadGateway();
}

std::string getTimeStamp()
{
    return std::to_string( std::time( 0 ) );
}
