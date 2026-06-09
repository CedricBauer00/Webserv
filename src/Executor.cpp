#include "../inc/Executor.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Writer.hpp"
#include "../inc/PageHandler.hpp"

Executor::Executor(const AEventHandler& handler,
	HttpParser&& parser,
	std::function<const IWebservModule::Srv*(const std::string&)>&& selectServer)
    : AEventHandler(_dupFd(handler.getFd()),
        handler.getServers(),
        handler.getEpoller(),
        EPOLLOUT | EPOLLRDHUP | EPOLLET),
		_parser(std::move(parser)),
		_selectServer(std::move(selectServer)) {
	std::cout << "FD " << _fd << ": [Executor] created" << std::endl;
}

Executor::~Executor() {
	std::cout << "FD " << _fd << ": [Executor] destroyed" << std::endl;
}

const IWebservModule::LocNode*	Executor::selectLocation(const std::string& path,
	const IWebservModule::LocNode& root) {
	const IWebservModule::LocNode* bestMatch = nullptr;
	std::deque<const IWebservModule::LocNode*> queue;
	queue.push_back(&root);
	while (!queue.empty()) {
		const IWebservModule::LocNode* node = queue.front();
		queue.pop_front();
		if (node->matchType == 0 && node->name == path)
			return node;
		else if (path.compare(0, node->name.size(), node->name) == 0) {
			if (bestMatch == nullptr || node->name.size() > bestMatch->name.size())
				bestMatch = node;
		}
		for (const auto& loc : node->locations)
			queue.push_back(loc.get());
	}
	return bestMatch;
}

void	Executor::process(uint32_t events) {
	if (events & (EPOLLERR | EPOLLHUP)) {
 		_printSocketError();
        std::cerr << "FD " << _fd
		<< ": [Writer] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		try {
			Method m;
	
			auto server = _selectServer(_parser.getHostName()); // select server based on Host name
			if (!server->srvConfs.empty()) {
				for (const auto& item :
					dynamic_cast<WebservCoreParser::SrvCoreConf*>(
						server->srvConfs[0].get())->serverNames) {
					std::cout << item << " ";
				}
				std::cout << '\n';
			}
			const IWebservModule::LocNode* loc = selectLocation(
				_parser.getPath(), *server->location.get()); // select location based on URI
			std::cout << "Selected location: '" << loc->name 
			<< "' with match type " << loc->matchType << "\n";
	
			whichMethod whichMethod = _parser.getMethod();
	
			if (!loc->locConfs.empty()) {
				const auto* locConf =\
				dynamic_cast<WebservCoreParser::LocCoreConf*>(loc->locConfs[0].get());
				if (!(_parser.getReqMethod() & locConf->allowedMethods)) {
					std::cerr << "Requested method not allowed" << "\n";
					throw MethodNotAllowed();
				}
			}
	
			std::string joinedPath = m.joinRootAndPath(_parser.getPath(), whichMethod, *loc);
			std::cout << "joinedPath: " << joinedPath << std::endl;
	
			if ( whichMethod == METHOD_GET )
				m.getMethod( joinedPath, _res, *loc ); // && if GET method is allowed
			else if ( whichMethod == METHOD_DELETE )
				m.deleteMethod( joinedPath, _res, *loc ); // && if DELETE method is allowed
			if ( whichMethod == METHOD_POST )
			{
				_parser.setBody(); // for POST requests - last step of execution
				m.postMethod( joinedPath, _res, _parser.getBody(), *loc ); // && if POST method is allowed
				std::cout << ORANGE << _parser.getBody() << RESET << std::endl;
			}
			/// Response Buidling 
			_res.build();
			std::cout << ORANGE << _res.getResponse() << RESET << std::endl;
			new Writer(*this, std::move(_res));
		}
		catch ( const HttpException& e )
		{
			PageHandler pageHandler( e.getStatusCode(), e.getReasonPhrase() );
			if ( e.getStatusCode() == 301 || e.getStatusCode() == 302 )
			{
				pageHandler.setRedirectPage( _res, e.getLocation() );
			}
			else
				pageHandler.setErrorPage( _res );
			new Writer(*this, std::move(_res));
		}
	}
	catch (const std::exception& e) {
		int fd = getFd();
		throw std::runtime_error("FD " + std::to_string(fd)
		+ ": [Executor] Error executing request," + e.what());
	}
	delete this; // Execution finished, destroy self
}