#pragma once

#include "Executor.hpp"
#include "Response.hpp"
#include "HttpParser.hpp"

class BodyReader: public AEventHandler {
	private:
		const size_t	BUFFER_SIZE{4096};
		HttpParser		_parser;
		Response		_res;

		void	_receiveFromClient();

	public:
		BodyReader() = delete;
		BodyReader(AEventHandler& handler, HttpParser&& parser);
		virtual ~BodyReader();

		void	process(uint32_t events) override;
};