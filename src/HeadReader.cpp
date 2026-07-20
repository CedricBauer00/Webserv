#include "../inc/HeadReader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Executor.hpp"
#include "../inc/Writer.hpp"

HeadReader::HeadReader(const Listener& listener)
: AEventHandler(acceptConn(listener.getFd()),
	EPOLLIN | EPOLLRDHUP | EPOLLET,
	listener.epoller,
	listener.timer,
	listener.selectSrv)
, _parser(std::make_unique<HttpHeaderParser>())
, _res(std::make_unique<Response>()) {
	timer->setHandler(this);
	// char	s[INET_ADDRSTRLEN];

	// inet_ntop(_sock.ss->ss_family, getInAddr(*_sock.ss), s, sizeof s);
	// std::cout << "FD " << _sock.fd << ": [HeadReader] accepted connection from "
	// << s << ":" << ntohs(getPort(*_sock.ss)) << std::endl;
}

HeadReader::HeadReader(AEventHandler&& handler,
	std::string&& request)
: AEventHandler(std::move(handler))
, _parser(std::make_unique<HttpHeaderParser>(std::move(request)))
, _res(std::make_unique<Response>()) {
	_parser->parse(nullptr, 0); //consume header remaining from previous parsing
	if (_parser->parseCompleted()) {
		_parser->unsetParseCompleted();
		_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
		new Executor(std::move(*this), std::move(_parser), std::move(_res));
		delete this;
	}
}

HeadReader::~HeadReader() {
    // std::cout << "FD " << _sock.fd << ": [HeadReader] destroyed" << std::endl;
}

WebservSocket	HeadReader::acceptConn(int listenFd) {
	socklen_t					addrLen{sizeof(struct sockaddr_storage)};
	WebservSocket				ret(-1, std::make_unique<struct sockaddr_storage>());

	ret.fd = accept(
		listenFd, reinterpret_cast<struct sockaddr*>(ret.ss.get()), &addrLen);
    if (ret.fd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw wouldBlockException(); // No more incoming connections to accept
		throw std::runtime_error(std::string("FD ")
		+ std::to_string(listenFd) + ": [HeadReader] " + strerror(errno));
	}
    _setNonBlocking(ret.fd);
	return ret;

}

void	HeadReader::_receiveFromClient() {
    //  std::cout << BLUE << "FD " << _sock.fd << ": [HeadReader] Reading from client.."
    //  << RESET << std::endl;
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t count = recv(_sock.fd, buffer, sizeof(buffer), 0);
        if (0 < count) {
			_parser->parse(buffer, static_cast<std::size_t>(count));
			if (_parser->parseCompleted()) {
				_parser->unsetParseCompleted();
				return; // Header fully received
			}
			continue;
        }
        if (count == 0)
            throw std::runtime_error(
				std::string("FD ") + std::to_string(_sock.fd)
				+ ": [HeadReader] Client disconnected before completing header");
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw wouldBlockException();
        if (errno == EINTR)
            continue;
        throw std::runtime_error(
				std::string("FD ") + std::to_string(_sock.fd)
				+ ": [HeadReader] Error receiving from client, " + strerror(errno));
    }
}

void    HeadReader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
		_printSocketError();
		std::cerr << "FD " << _sock.fd
		<< ": [HeadReader] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try {
		try {
			_receiveFromClient();
			_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
			new Executor(std::move(*this), std::move(_parser), std::move(_res));
		}
		catch (HttpException& e) {
			_res->build(std::move(e));
			_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
			new Writer(std::move(*this), std::move(_parser), std::move(_res));
		}
	}
    catch (const wouldBlockException& e) {
		_lastActivity = std::chrono::steady_clock::now();
		return; // Nothing more to read now
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	delete this; // Will also remove from epoll
}