#pragma once

#include "Listener.hpp"
#include "Response.hpp"
#include "HttpParser.hpp"

class HeadReader: public AEventHandler {
	private:
		const size_t	BUFFER_SIZE{4096};
		HttpParser		_parser;
        Response        _res;

		int			_acceptConn(int listenFd);
		void		_receiveFromClient();

	public:
		HeadReader() = delete;
		HeadReader(const Listener& listener);
		virtual ~HeadReader();

		void				process(uint32_t events) override;
};