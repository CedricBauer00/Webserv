#include "../inc/Executor.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Writer.hpp"
#include "../inc/BodyReader.hpp"
#include "../inc/PageHandler.hpp"

Executor::Executor(const AEventHandler& handler,
	HttpParser&& parser,
	std::function<const Srv*(const std::string&)>&& selectServer)
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

const LocNode*	Executor::selectLocation(const std::string& path,
	const LocNode& root) {
	const LocNode* bestMatch = nullptr;
	std::deque<const LocNode*> queue;
	queue.push_back(&root);
	while (!queue.empty()) {
		const LocNode* node = queue.front();
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
		<< ": [Executor] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		try {
			Method m;
	
			auto server = _selectServer(_parser.getHostName()); // select server based on Host name
			if (!server->srvConfs.empty()) {
                std::cout << "server_name: ";
				for (const auto& item :
					dynamic_cast<SrvCoreConf*>(
						server->srvConfs[0].get())->serverNames) {
					std::cout << item << " ";
				}
				std::cout << '\n';
			}
			const LocNode* loc = selectLocation(
				_parser.getPath(), *server->location.get()); // select location based on URI
			std::cout << "Selected location: '" << loc->name 
			<< "' with match type " << loc->matchType << "\n";
	
			whichMethod whichMethod = _parser.getMethod();
	
			if (!loc->locConfs.empty() && loc->locConfs[0].get() != nullptr) {
				auto* locConf =\
				dynamic_cast<const LocCoreConf*>(loc->locConfs[0].get());
				if (!(_parser.getReqMethod() & locConf->allowedMethods.value())) {
					std::cerr << "Requested method not allowed" << "\n";
					throw MethodNotAllowed();
				}
			}
	
			std::string joinedPath = m.joinRootAndPath(_parser.getPath(), whichMethod, *loc);
			std::cout << "joinedPath: " << joinedPath << std::endl;
			
			if (whichMethod == METHOD_POST) {
				if (_parser.headerHasContlen() && MAX_BODY_SIZE < _parser.getContlen())
					throw PayloadTooLarge();
				_parser.parseBody(nullptr, 0);
				if (_parser.bodyStopReceived()) {
					_res.build();
					new Writer(*this, std::move(_res));
				}
				else
					new BodyReader(*this, std::move(_parser));
			}
			else {
				if ( whichMethod == METHOD_GET )
					m.getMethod( joinedPath, _res, *loc ); // && if GET method is allowed
				else if ( whichMethod == METHOD_DELETE )
					m.deleteMethod( joinedPath, _res, *loc ); // && if DELETE method is allowed
				/// Response Buidling 
				_res.build();
				new Writer(*this, std::move(_res));
				std::cout << ORANGE << _res.getResponse() << RESET << std::endl;
			}
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
			_res.build();
			new Writer(*this, std::move(_res));
		}
	}
	catch (const std::exception& e) {
		std::cerr << "FD " << _fd
		<< ": [Executor] Error, " << e.what() << std::endl;
	}
	delete this; // Execution finished, destroy self
}