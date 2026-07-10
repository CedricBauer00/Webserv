#include "AHttpParser.hpp"

class	HttpEOFBodyParser : public AHttpParser {
	public:
		HttpEOFBodyParser();
		HttpEOFBodyParser(AHttpParser&& other, std::size_t max_size);
		virtual ~HttpEOFBodyParser();

		void	parse(char* buffer, std::size_t count) override;
};
