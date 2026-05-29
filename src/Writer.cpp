#include "../inc/Writer.hpp"
#include "../inc/Epoller.hpp"
#include "../inc/constants.h"
#include "../inc/Execution.hpp"

Writer::Writer(const Reader& reader)
    : AEventHandler(_dupFd(reader.getFd()),
        reader.getServers(),
        reader.getEpoller(),
        EPOLLOUT | EPOLLRDHUP | EPOLLET) {
	try {
		if(!reader.getcomplHeader())
			throw std::runtime_error("Header not complete");
		Execution e;
		e.execution(reader.getRequest(), _res, reader.getServers());
	}
	catch (const std::exception& e) {
		int fd = getFd();
		delete this;
		throw std::runtime_error("FD " + std::to_string(fd)
		+ ": [Writer] Error executing request," + e.what());
	}
}

Writer::~Writer() {
    std::cout << "FD " << _fd << ": [Writer] destroyed" << std::endl;
}

int	Writer::_dupFd(int readerFd) {
	int fd = dup(readerFd);

	if (fd == -1) {
		throw std::runtime_error(std::string("FD ")
		+ std::to_string(readerFd) + ": [Writer] " + strerror(errno));
	}
	std::cout << "FD " << fd 
	<< ": [Writer] Reader fd duplicated" << std::endl;
	return fd;
}

void	Writer::_sendToClient() {
	 std::cout << GREEN << "FD " << _fd
	 << ": [Writer] Sending response to client.." << RESET << std::endl;
	size_t total = _res.getResponse().size();
	while (_sentBytes < total) {
		ssize_t count = send(_fd,
			_res.getResponse().c_str() + _sentBytes,
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
