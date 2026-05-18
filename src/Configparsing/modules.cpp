#include "../../inc/Configparsing/modules.hpp"
#include "../../inc/Configparsing/ConfigParser.hpp"
// #include <algorithm>
// #include <cctype>

// static bool isDigits(const std::string& value) {
//     return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit);
// }

// static bool isValidIpv4Segment(const std::string& segment) {
//     if (segment.empty() || segment.size() > 3)
//         return false;
//     if (!isDigits(segment))
//         return false;
//     unsigned long num = std::stoul(segment);
//     return num <= 255;
// }

// static bool isValidIpv4(const std::string& address) {
//     size_t start = 0;
//     int parts = 0;

//     while (start <= address.size()) {
//         size_t end = address.find('.', start);
//         if (end == std::string::npos)
//             end = address.size();
//         std::string part = address.substr(start, end - start);
//         if (!isValidIpv4Segment(part))
//             return false;
//         ++parts;
//         if (end == address.size())
//             break;
//         start = end + 1;
//     }
//     return parts == 4;
// }

// static unsigned int parsePort(const std::string& value) {
//     if (!isDigits(value))
//         throw std::runtime_error("Invalid listen port '" + value + "'");
//     if (value.size() > 10)
//         throw std::runtime_error("Invalid listen port '" + value + "'");
//     unsigned long long port = std::stoull(value);
//     if (port > 65535)
//         throw std::runtime_error("Listen port must be in range 0..65535");
//     return static_cast<unsigned int>(port);
// }

// static WebservAddr parseListenValue(const std::string& value) {
//     WebservAddr addr;
//     size_t colonPos = value.find(':');

//     if (colonPos == std::string::npos) {
//         if (isValidIpv4(value)) {
//             addr.ip = value;
//             return addr;
//         }
//         addr.port = value;
//         parsePort(addr.port); // validate
//         return addr;
//     }

//     addr.ip = value.substr(0, colonPos);
//     addr.port = value.substr(colonPos + 1);
//     if (!isValidIpv4(addr.ip))
//         throw std::runtime_error("Invalid listen IP '" + addr.ip + "'");
//     parsePort(addr.port);
//     return addr;
// }

