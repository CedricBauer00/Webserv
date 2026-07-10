#pragma once

#include "Listener.hpp"
#include "Response.hpp"
#include "HttpHeaderParser.hpp"

class HeadReader: public AEventHandler {
	private:
		const size_t	    			BUFFER_SIZE{4096};
		std::unique_ptr<AHttpParser>	_parser;
		std::unique_ptr<Response>		_res;

		void		_receiveFromClient();

	public:
		HeadReader() = delete;
		HeadReader(const Listener& listener);
		virtual ~HeadReader();

		static WebservSocket	acceptConn(int listenFd);

		void	process(uint32_t events) override;
};