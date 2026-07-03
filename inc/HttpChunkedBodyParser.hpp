#include "AHttpParser.hpp"

class	HttpChunkedBodyParser : public AHttpParser {
	public:
		void	parse(char* buffer, std::size_t count);
};
