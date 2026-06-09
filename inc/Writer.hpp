#pragma once

#include "HeadReader.hpp"
#include "Response.hpp"

class Writer: public AEventHandler {
    private:
        Response	_res;
        size_t		_sentBytes{0};

		void	_sendToClient();

    public:
        Writer() = delete;
        Writer(const AEventHandler& handler,
			Response&& res);
        virtual ~Writer();

        void	process(uint32_t events) override;
};