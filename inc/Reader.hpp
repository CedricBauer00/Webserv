#pragma once

#include "Listener.hpp"

class Reader: public AEventHandler {
	private:
		std::string	req;

		int	receiveFromClient();
	public:
		Reader() = delete;
		Reader(const int fd, const Listener& listener);
		virtual ~Reader();

		void	process() override;
};