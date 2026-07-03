#include "AHttpParser.hpp"

class	HttpHeaderParser : public AHttpParser {
	public:
		void	parse(char* buffer, std::size_t count);
};
