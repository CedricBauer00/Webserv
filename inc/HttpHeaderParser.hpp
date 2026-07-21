#include <arpa/inet.h>
#include "AHttpParser.hpp"

class	HttpHeaderParser : public AHttpParser {
	private:
		void	_checkProtocol();
		void	_setMethod();
		int		_hexValue(char c);
		void	_decodePercentEncoding(std::string& requestTarget);
		void	_splitRequestTarget(std::string& requestTarget);
		void	_normalizePath();
		void	_validatePort(const std::string& port);
		void	_validateHostName(std::string_view hostName);
		void	_validateHostHeader(const std::string& value );

	public:
		HttpHeaderParser();
		HttpHeaderParser(std::string&& request);
		HttpHeaderParser(AHttpParser&& other);
		virtual ~HttpHeaderParser();

		void	parse(char* buffer, std::size_t count) override;
};
