#include "AHttpParser.hpp"

class	HttpEOFBodyParser : public AHttpParser {
	public:
		void	parse(char* buffer, std::size_t count);
};
