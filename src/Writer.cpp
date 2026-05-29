#include "../inc/Writer.hpp"
#include "../inc/Epoller.hpp"

Writer::Writer(const int fd, const Reader& reader)
    : AEventHandler(fd,
        reader.getServers(),
        reader.getEpoller(),
        EPOLLOUT | EPOLLRDHUP | EPOLLET),
        _res(reader.getResponse()) {
}

Writer::~Writer() {
    std::cout << "FD " << _fd << ": [Writer] destroyed" << std::endl;
}

void    Writer::process(uint32_t events) {
    if (events & (EPOLLERR | EPOLLHUP)) {
        int fd = getFd();
 		_printSocketError();
        delete this;
        throw std::runtime_error(
            "FD " + std::to_string(fd) + ": Client disconnected unexpectedly");
    }

    if (events & EPOLLOUT) {
        size_t total = _res.getResponse().size();
        while (_sentBytes < total) {
            ssize_t count = send(_fd,
                _res.getResponse().c_str() + _sentBytes,
                total - _sentBytes,
                0);
            if (count == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    return; // Can't send more right now
                if (errno == EINTR)
                    continue; // Interrupted, try again
                std::cerr << "FD " << getFd() 
                << ": [Writer] Error sending response, "
                << strerror(errno) << std::endl;
                delete this; // Will also remove from epoll
                throw std::runtime_error(
                    "FD " + std::to_string(getFd()) + ": Error sending response");
            }
            _sentBytes += static_cast<size_t>(count);
        }
        delete this; // Response fully sent, close connection
    }
}
