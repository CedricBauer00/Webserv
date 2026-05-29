#pragma once

#include "Reader.hpp"
#include "Response.hpp"

class Writer: public AEventHandler {
    private:
        Response	_res;
        size_t		_sentBytes{0};

        int		_dupFd(int readerFd);
		void	_sendToClient();
    public:
        Writer() = delete;
        Writer(const Reader& reader);
        virtual ~Writer();

        void	process(uint32_t events) override;
};