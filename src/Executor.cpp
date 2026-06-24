#include "../inc/Executor.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Writer.hpp"
#include "../inc/BodyReader.hpp"

Executor::Executor(AEventHandler& handler,
	HttpParser&& parser,
	std::function<const Srv*(const std::string&)>&& selectServer)
    : AEventHandler(handler.getFd(),
        handler.getServers(),
        handler.getEpoller(),
        EPOLLOUT | EPOLLRDHUP | EPOLLET,
		Epoller::EpollOperation::Modify),
	_parser(std::move(parser)),
	_selectServer(std::move(selectServer)) {
	handler.setFd(-1);
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
	if (2 < _loc->locConfs.size())
		_locRedirectConf =\
			dynamic_cast<const LocRedirectConf*>(_loc->locConfs[2].get());
}

void	Executor::_selectLocation(const LocNode& root)
{
	_loc = nullptr;
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
	_fsPath = std::filesystem::current_path().string();
}

void	Executor::_assertHttpMethodAllowed() {
	if (!(_parser.getMethodMask() & *_locCoreConf->allowedMethods)) {
		std::cerr << "Requested method not allowed" << "\n";
		throw MethodNotAllowed({{"ALLOW", std::string("GET, POST")}});
	}
}

void	Executor::_resolveFilesystemPath() {
	if (*_locCoreConf->alias)
		_fsPath = _locCoreConf->root\
			+ _parser.getPath().substr(_loc->name.size());
	else 
		_fsPath = _locCoreConf->root + _parser.getPath();
	// std::cout << "FilesystemPath: " << _fsPath << std::endl;
}

void	Executor::process(uint32_t events) {
	if (events & (EPOLLERR | EPOLLHUP)) {
 		_printSocketError();
        std::cerr << "FD " << _fd
		<< ": [Executor] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		auto server = _selectServer(_parser.getHostName()); // select server based on Host name
		if (!server->srvConfs.empty()) {
			// std::cout << "server_name: ";
			// for (const auto& item :
			// 	dynamic_cast<SrvCoreConf*>(
			// 		server->srvConfs[0].get())->serverNames) {
			// 	std::cout << item << " ";
			// }
			// std::cout << '\n';
		}
		while (true) {
			try {
				Method m;

				_selectLocation(*server->location.get()); // select location based on URI path
				// std::cout << "Selected location: '" << _loc->name << "' with match type " << _loc->matchType << "\n";

				if (_locRedirectConf) {
					std::string		path = _locRedirectConf->retDirective.value().path;
					unsigned long	code = _locRedirectConf->retDirective.value().statusCode;

					if (path.size())
						_res.build({{"Location", path}}, std::to_string(code), statusCodeToReasonPhrase.at(code));
					else
						_res.build(std::to_string(code), statusCodeToReasonPhrase.at(code));
					new Writer(*this, std::move(_res));
					break;
				}

				if (_locCoreConf) {
					_assertHttpMethodAllowed();
					_resolveFilesystemPath();
				}
				else
					_setWorkingDirAsFilesystemPath();

				method whichMethod = _parser.getMethod();
				if (whichMethod == METHOD_POST) {
					if (_parser.headerHasContlen() && MAX_BODY_SIZE < _parser.getContlen())
						throw PayloadTooLarge();
					_parser.parseBody(nullptr, 0); //consume body remaining from header parsing
					if (_parser.bodyStopReceived()) {
						m.postMethod(_fsPath, _res, _parser);
						new Writer(*this, std::move(_res));
					}
					else {
						new BodyReader(*this, std::move(_parser));
					}
					break;
				}
				
				if (whichMethod == METHOD_GET) {
					if (!m.getMethod(_fsPath, _res, _parser, _locIndexConf))
						continue;
				}
				else
					m.deleteMethod(_fsPath, _res, _parser);
				new Writer(*this, std::move(_res));
			}
			catch ( HttpException& e )
			{
				if (_locCoreConf) {
					auto it = _locCoreConf->errPages.find(e.getStatusCode());
					if (it != _locCoreConf->errPages.end()
					&& it->second.path != _parser.getPath()) {
						_res.setRedirect(it->second.resCode);
						_parser.setRedirectPath(std::string(it->second.path));
						continue;
					}
				}
				_res.build(std::move(e));
				new Writer(*this, std::move(_res));
			}
			break;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "FD " << _fd
		<< ": [Executor] Error, " << e.what() << std::endl;
	}
	delete this; // Execution finished, destroy self
}