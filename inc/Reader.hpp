#pragma once

#include "Listener.hpp"

class Reader: public AEventHandler {
	private:
		const size_t	BUFFER_SIZE{4096};
		std::string		_request;
		bool			_complHeader;

		int		_receiveFromClient();

	public:
		Reader() = delete;
		Reader(const int fd, const Listener& listener);
		virtual ~Reader();

		void	process(uint32_t ev) override;
};