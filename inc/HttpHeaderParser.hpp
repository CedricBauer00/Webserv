#include "AHttpParser.hpp"

class	HttpHeaderParser : public AHttpParser {
	public:
		HttpHeaderParser();
		virtual ~HttpHeaderParser();

		void	parse(char* buffer, std::size_t count) override;
};
