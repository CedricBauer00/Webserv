#include "AHttpParser.hpp"

class	HttpHeaderParser : public AHttpParser {
	public:
		HttpHeaderParser();
		HttpHeaderParser(std::string&& request);
		HttpHeaderParser(AHttpParser&& other);
		virtual ~HttpHeaderParser();

		void	parse(char* buffer, std::size_t count) override;
};
