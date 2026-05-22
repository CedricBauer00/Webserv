#pragma once

#include "Listener.hpp"

class Reader: public AEventHandler {
	private:
		std::string	req;
	public:
		Reader(Listener& listener);
		virtual ~Reader();
};