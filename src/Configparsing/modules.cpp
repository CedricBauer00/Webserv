#include <algorithm>
#include "../../inc/Configparsing/modules.hpp"

WebservSocket::WebservSocket() noexcept {
}

WebservSocket::WebservSocket(
    int _fd, std::unique_ptr<struct sockaddr_storage>&& _ss) noexcept
: fd(_fd), ss(std::move(_ss)) {
	memset(ss.get(), 0, sizeof *ss);
}

WebservSocket::WebservSocket(WebservSocket&& other) noexcept
: fd(other.fd), ss(std::move(other.ss)) {
    other.fd = -1;
}

WebservSocket::~WebservSocket() {
	if (fd != -1) {
		if (close(fd) == -1)
			std::cerr << "FD " << fd << ": " << strerror(errno) << std::endl;
		std::cout << "FD " << fd << ": closed" << std::endl;
	}
}

bool	isDigits(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

static bool	isValidIpv4Segment(const std::string& segment) {
    if (segment.empty() || 3 < segment.size() || !isDigits(segment))
        return false;
    unsigned long	num = std::stoul(segment);
    return num <= 255;
}

bool	isValidIpv4(const std::string& address) {
    size_t start = 0;
    int parts = 0;

    while (start <= address.size()) {
        size_t end = address.find('.', start);
        if (end == std::string::npos)
            end = address.size();
        std::string part = address.substr(start, end - start);
        if (!isValidIpv4Segment(part))
            return false;
        ++parts;
        if (end == address.size())
            break;
        start = end + 1;
    }
    return parts == 4;
}

bool	isValidPort(const std::string& str) {
    if (!isDigits(str))
        return false;
    unsigned long	num = std::stoul(str);
    if (65535 < num)
        return false;
    return true;
}
