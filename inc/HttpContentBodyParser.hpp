#include "AHttpParser.hpp"

class	HttpContentBodyParser : public AHttpParser {
	private:
		std::size_t	_contentLength{0};

	public:
		HttpContentBodyParser();
		HttpContentBodyParser(AHttpParser&& other, std::size_t max_size);
		virtual ~HttpContentBodyParser();

		void	parse(char* buffer, std::size_t count) override;
};
