#include "../inc/Writer.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"

Writer::Writer(AEventHandler& handler,
	Response&& res)
    : AEventHandler(handler.getFd(),
        handler.getServers(),
        handler.getEpoller(),
        EPOLLOUT | EPOLLRDHUP | EPOLLET,
        Epoller::EpollOperation::Modify),
    _res(std::move(res)) {
	std::cout << "FD " << _fd << ": [Writer] created" << std::endl;
}

Writer::~Writer() {
    std::cout << "FD " << _fd << ": [Writer] destroyed" << std::endl;
}

void	Writer::_sendToClient() {
	 std::cout << GREEN << "FD " << _fd
	 << ": [Writer] Sending response to client.." << RESET << std::endl;
	size_t total = _res.getText().size();
	while (_sentBytes < total) {
		ssize_t count = send(_fd,
			_res.getText().c_str() + _sentBytes,
			total - _sentBytes,
			0);
		if (count == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				throw wouldBlockException();
			if (errno == EINTR)
				continue; // Interrupted, try again
			//TODO: have to supress SIGPIPE
			throw std::runtime_error(std::string("FD ") + std::to_string(_fd)
			+ ": [Writer] Client disconnected, send did not finish");
		}
		_sentBytes += static_cast<size_t>(count);
	}
	if (_res.bodySize()) {
		_res.mvBodyToText();
		_sentBytes = 0;
		_sendToClient();
	}
}

void    Writer::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
 		_printSocketError();
        std::cerr << "FD " << _fd
		<< ": [Writer] Client disconnected unexpectedly" << std::endl;
		delete this;
    }

	try{
		_sendToClient();
	}
	catch (const wouldBlockException& e) {
		return; // Can't send more right now
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	delete this; // Will also remove from epoll
}
