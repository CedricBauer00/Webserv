#include "../inc/BodyReader.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Executor.hpp"

BodyReader::BodyReader(const AEventHandler& handler,
	HttpParser&& parser)
    : AEventHandler(_dupFd(handler.getFd()),
        handler.getServers(),
        handler.getEpoller(),
        EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET),
		_parser(std::move(parser)) {
}

BodyReader::~BodyReader() {
    std::cout << "FD " << _fd << ": [BodyReader] destroyed" << std::endl;
}

void	BodyReader::_receiveFromClient() {
     std::cout << BLUE << "FD " << _fd << ": [BodyReader] Reading from client.." 
     << RESET << std::endl;
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t count = recv(_fd, buffer, sizeof(buffer), 0);
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
				std::string("FD ") + std::to_string(_fd)
				+ ": [BodyReader] Client disconnected before completing body");
		}
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw wouldBlockException();
        if (errno == EINTR)
            continue;
        throw std::runtime_error(
				std::string("FD ") + std::to_string(_fd)
				+ ": [BodyReader] Error receiving from client, " + strerror(errno));
    }
}

void    BodyReader::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
		_printSocketError();
		std::cerr << "FD " << _fd
		<< ": [BodyReader] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try{
		try {
			_receiveFromClient();
			std::cout << _parser.getBody() << std::endl;
			new Executor(*this, std::move(_parser), _selectServerFactory());
		}
		catch (const HttpException& e) {
			std::cerr << "FD " << _fd << ": [BodyReader] HTTP error: " << std::endl;
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