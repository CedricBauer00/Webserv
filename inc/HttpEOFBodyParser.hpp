#include "AHttpParser.hpp"

class	HttpEOFBodyParser : public AHttpParser {
	public:
		HttpEOFBodyParser();
		virtual ~HttpEOFBodyParser();

		void	parse(char* buffer, std::size_t count);
};
