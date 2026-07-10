#pragma once

#include "Executor.hpp"
#include "Response.hpp"
#include "AHttpParser.hpp"

class BodyReader: public AEventHandler {
	private:
		const size_t					BUFFER_SIZE{4096};
		std::unique_ptr<AHttpParser>	_parser;
		std::unique_ptr<Response>		_res;

		void	_receiveFromClient();

	public:
		BodyReader() = delete;
		BodyReader(AEventHandler&& handler,
			std::unique_ptr<AHttpParser>&& parser,
			std::unique_ptr<Response>&& res);
		virtual ~BodyReader();

		void	process(uint32_t events) override;
};