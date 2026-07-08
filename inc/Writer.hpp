#pragma once

#include "HeadReader.hpp"
#include "Response.hpp"

class Writer: public AEventHandler {
    private:
		size_t							_sentBytes{0};
		std::unique_ptr<AHttpParser>	_parser;
        Response						_res;

		void	_sendToClient();

    public:
        Writer() = delete;
        Writer(AEventHandler&& handler,
            std::unique_ptr<AHttpParser>&& parser,
            Response&& res);
        virtual ~Writer();

        void	process(uint32_t events) override;
};