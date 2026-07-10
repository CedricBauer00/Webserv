#include "../inc/Executor.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Writer.hpp"
#include "../inc/BodyReader.hpp"
#include "../inc/HttpChunkedBodyParser.hpp"
#include "../inc/HttpContentBodyParser.hpp"
#include "../inc/HttpEOFBodyParser.hpp"

Executor::Executor(AEventHandler&& handler,
	std::unique_ptr<AHttpParser>&& parser,
	std::unique_ptr<Response>&& res)
: AEventHandler(std::move(handler))
, _parser(std::move(parser))
, _res(std::move(res)) {
	// std::cout << "FD " << _sock.fd << ": [Executor] created" << std::endl;
}

Executor::~Executor() {
	// std::cout << "FD " << _sock.fd << ": [Executor] destroyed" << std::endl;
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
	auto& uriPath = _parser->getPath();
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
	_fsPath = std::filesystem::current_path().string() + _parser->getPath();
}

void	Executor::_assertHttpMethodAllowed() {
	if (!(_parser->getMethodMask() & *_locCoreConf->allowedMethods)) {
		std::string	val;

		for (const auto& item : methodMap) {
			if (*_locCoreConf->allowedMethods & item.second.first) {
				if (val.size())
					val.append(", ");
				val.append(item.first);
			}
		}
		throw MethodNotAllowed({{"Allow", std::move(val)}});
	}
}

void	Executor::_resolveFilesystemPath() {
	if (*_locCoreConf->alias)
		_fsPath = _locCoreConf->root\
			+ _parser->getPath().substr(_loc->name.size());
	else 
		_fsPath = _locCoreConf->root + _parser->getPath();
	// std::cout << "FilesystemPath: " << _fsPath << std::endl;
}

void	Executor::process(uint32_t events) {
	if (events & (EPOLLERR | EPOLLHUP)) {
 		_printSocketError();
        std::cerr << "FD " << _sock.fd
		<< ": [Executor] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		auto server = selectSrv(_parser->getHostName()); // select server based on Host name
		// if (!server->srvConfs.empty()) {
		// 	std::cout << "server_name: ";
		// 	for (const auto& item :
		// 		dynamic_cast<SrvCoreConf*>(
		// 			server->srvConfs[0].get())->serverNames) {
		// 		std::cout << item << " ";
		// 	}
		// 	std::cout << '\n';
		// }
		while (true) {
			try {
				Method m;

				_selectLocation(*server->location.get()); // select location based on URI path
				// std::cout << "Selected location: '" << _loc->name << "' with match type " << _loc->matchType << "\n";

				if (_locRedirectConf) {
					std::string		path = _locRedirectConf->retDirective.value().path;
					unsigned long	code = _locRedirectConf->retDirective.value().statusCode;

					if (path.size())
						_res->build({{"Location", path}}, std::to_string(code), statusCodeToReasonPhrase.at(code));
					else
						_res->build(std::to_string(code), statusCodeToReasonPhrase.at(code));
					_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
					new Writer(std::move(*this), std::move(_parser), std::move(_res));
					break;
				}

				if (_locCoreConf) {
					_assertHttpMethodAllowed();
					_resolveFilesystemPath();
				}
				else
					_setWorkingDirAsFilesystemPath();

				method whichMethod = _parser->getMethod();
				if (whichMethod == METHOD_POST) {
					std::unique_ptr<AHttpParser> bodyParser;
					std::size_t	maxBodySize = std::numeric_limits<std::size_t>::max();

					if (_locCoreConf)
						maxBodySize = _locCoreConf->clientMaxBodySize.value();
					if (_parser->headerHasChunked())
						bodyParser = std::make_unique<HttpChunkedBodyParser>(std::move(*_parser), maxBodySize);
					else if (_parser->headerHasContlen())
						bodyParser = std::make_unique<HttpContentBodyParser>(std::move(*_parser), maxBodySize);
					else
						bodyParser = std::make_unique<HttpEOFBodyParser>(std::move(*_parser), maxBodySize);
					bodyParser->parse(nullptr, 0); //consume body remaining from header parsing
					if (bodyParser->parseCompleted()) {
						m.postMethod(_fsPath, *_res, *bodyParser);
						_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
						new Writer(std::move(*this), std::move(bodyParser), std::move(_res));
					}
					else {
						_modifyEvent(EPOLLIN | EPOLLRDHUP | EPOLLET);
						new BodyReader(std::move(*this), std::move(bodyParser), std::move(_res));
					}
					break;
				}
				
				if (whichMethod == METHOD_GET) {
					if (!m.getMethod(_fsPath, *_res, *_parser, _locIndexConf))
						continue;
				}
				else
					m.deleteMethod(_fsPath, *_res, *_parser);
				_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
				new Writer(std::move(*this), std::move(_parser), std::move(_res));
			}
			catch ( HttpException& e )
			{
				if (_locCoreConf) {
					auto it = _locCoreConf->errPages.find(e.getStatusCode());
					if (it != _locCoreConf->errPages.end()
					&& it->second.path != _parser->getPath()) {
						_res->setRedirect(it->second.resCode);
						if (300 <= it->second.resCode && it->second.resCode < 400) {
							e = HttpException(
								it->second.resCode,
								statusCodeToReasonPhrase.at(it->second.resCode),
								{{"Location", it->second.path}});
						}
						else {
							_parser->setRedirectPath(std::string(it->second.path));
							_parser->setMethod("GET");
							continue;
						}
					}
				}
				_res->build(std::move(e));
				_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
				new Writer(std::move(*this), std::move(_parser), std::move(_res));
			}
			break;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "FD " << _sock.fd
		<< ": [Executor] Error, " << e.what() << std::endl;
	}
	delete this; // Execution finished, destroy self
}