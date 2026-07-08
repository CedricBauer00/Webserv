#include "AHttpParser.hpp"

class	HttpContentBodyParser : public AHttpParser {
	public:
		HttpContentBodyParser();
		virtual ~HttpContentBodyParser();

		void	parse(char* buffer, std::size_t count);
};
