#include "AHttpParser.hpp"

class	HttpChunkedBodyParser : public AHttpParser {
    protected:
		std::size_t	_currentChunkSize{0};
		bool 		_waitingForChunkData{false};
		bool 		_waitingForLastChunkCRLF{false};

	public:
		HttpChunkedBodyParser(AHttpParser&& other, std::size_t max_size) noexcept;
		void	parse(char* buffer, std::size_t count) override;
};
