#include <algorithm>
#include "../../inc/Configparsing/modules.hpp"

WebservSocket::WebservSocket(WebservSocket&& other) noexcept
: fd(other.fd), st(std::move(other.st)) {
    other.fd = -1;
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
