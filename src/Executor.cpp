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

void	Executor::_resolveLocConfs() {
	if (!_loc)
		return;
	if (0 < _loc->locConfs.size())
		_locCoreConf = dynamic_cast<const LocCoreConf*>(_loc->locConfs[0].get());
	if (1 < _loc->locConfs.size())
		_locIndexConf = dynamic_cast<const LocIndexConf*>(_loc->locConfs[1].get());
}

void	Executor::_selectLocation(const LocNode& root)
{
	auto& uriPath = _parser.getPath();
	std::deque<const LocNode*> queue;

	queue.push_back(&root);
	while (!queue.empty()) {
		const LocNode* node = queue.front();
		queue.pop_front();
		if (node->matchType == 0 && node->name == uriPath) {
			_loc = node;
			_resolveLocConfs();
			return;
		}
		else if (uriPath.compare(0, node->name.size(), node->name) == 0) {
			if (_loc == nullptr || _loc->name.size() < node->name.size())
				_loc = node;
		}
		for (const auto& loc : node->locations)
			queue.push_back(loc.get());
	}
	_resolveLocConfs();
}

void	Executor::_setWorkingDirAsFilesystemPath() {
	_filesystemPath = std::filesystem::current_path().string();
}

void	Executor::_assertHttpMethodAllowed() {
	if (!(_parser.getMethodMask() & *_locCoreConf->allowedMethods)) {
		std::cerr << "Requested method not allowed" << "\n";
		throw MethodNotAllowed();
	}
}

void	Executor::_resolveFilesystemPath() {
	if (*_locCoreConf->alias)
		_filesystemPath = _locCoreConf->root\
			+ _parser.getPath().substr(_loc->name.size());
	else 
		_filesystemPath = _locCoreConf->root + _parser.getPath();
	std::cout << "FilesystemPath: " << _filesystemPath << std::endl;
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

			_selectLocation(*server->location.get()); // select location based on URI path
			std::cout << "Selected location: '" << _loc->name << "' with match type " << _loc->matchType << "\n";
	
			if (_locCoreConf) {
				_assertHttpMethodAllowed();
				_resolveFilesystemPath();
			}
			else
				_setWorkingDirAsFilesystemPath();

			method whichMethod = _parser.getMethod();
			if (whichMethod != METHOD_POST) {
				if (whichMethod == METHOD_GET)
					m.getMethod( _filesystemPath, _res, _parser, _locIndexConf);
				else
					m.deleteMethod(_filesystemPath, _res, _parser);
				new Writer(*this, std::move(_res));
			}
			else {
				if (_parser.headerHasContlen() && MAX_BODY_SIZE < _parser.getContlen())
					throw PayloadTooLarge();
				_parser.parseBody(nullptr, 0); //consume body remaining from header parsing
				if (_parser.bodyStopReceived()) {
					m.postMethod(_filesystemPath, _res, _parser);
					new Writer(*this, std::move(_res));
				}
				else
					new BodyReader(*this, std::move(_parser));
			}
		}
		catch ( const HttpException& e )
		{
			_res.build(std::to_string(e.getStatusCode()),
				std::string(e.getReasonPhrase()));
			new Writer(*this, std::move(_res));
		}
	}
	catch (const std::exception& e) {
		std::cerr << "FD " << _fd
		<< ": [Executor] Error, " << e.what() << std::endl;
	}
	delete this; // Execution finished, destroy self
}