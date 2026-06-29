#include "../inc/BodyReader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Executor.hpp"
#include "../inc/Writer.hpp"

BodyReader::BodyReader(AEventHandler&& handler,
	HttpParser&& parser,
	Response&& res)
: AEventHandler(std::move(handler))
, _parser(std::move(parser))
, _res(std::move(res)) {
	std::cout << "FD " << _sock.fd << ": [BodyReader] created" << std::endl;
}

BodyReader::~BodyReader() {
    std::cout << "FD " << _sock.fd << ": [BodyReader] destroyed" << std::endl;
}

void	BodyReader::_receiveFromClient() {
     std::cout << BLUE << "FD " << _sock.fd << ": [BodyReader] Reading from client.." 
     << RESET << std::endl;
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t count = recv(_sock.fd, buffer, sizeof(buffer), 0);
        if (0 < count) {
			_parser.parseBody(buffer, static_cast<std::size_t>(count));
			if (_parser.bodyStopReceived())
				return; // Body fully received
            continue;
        }
        if (count == 0) {
			if (_parser.bodyStopReceived() || _parser.isHTTP1p0())
				return; // Body fully received
            throw std::runtime_error(
				std::string("FD ") + std::to_string(_sock.fd)
				+ ": [BodyReader] Client disconnected before completing body");
		}
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw wouldBlockException();
        if (errno == EINTR)
            continue;
        throw std::runtime_error(
				std::string("FD ") + std::to_string(_sock.fd)
				+ ": [BodyReader] Error receiving from client, " + strerror(errno));
    }
}

void    BodyReader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
		_printSocketError();
		std::cerr << "FD " << _sock.fd
		<< ": [BodyReader] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try{
		try {
			_receiveFromClient();
			_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
			new Executor(std::move(*this), std::move(_parser), std::move(_res));
		}
		catch (HttpException& e) {
			_res.build(std::move(e));
			_modifyEvent(EPOLLOUT | EPOLLRDHUP | EPOLLET);
			new Writer(std::move(*this), std::move(_parser), std::move(_res));
		}
	}
    catch (const wouldBlockException& e) {
		return; // Nothing more to read now
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	delete this; // Will also remove from epoll
}