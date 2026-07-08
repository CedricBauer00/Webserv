#include "AHttpParser.hpp"

class	HttpChunkedBodyParser : public AHttpParser {
    protected:
		std::size_t	_currentChunkSize{0};
		bool 		_waitingForChunkData{false};
		bool 		_waitingForLastChunkCRLF{false};

	public:
		void	parse(char* buffer, std::size_t count);
};
