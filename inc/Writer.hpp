#pragma once

#include "Reader.hpp"
#include "Response.hpp"

class Writer: public AEventHandler {
    private:
        Response	_res;
        size_t		_sentBytes{0};
        HttpParser	_parser;

        int		_dupFd(int readerFd);
		void	_sendToClient();
    public:
        Writer() = delete;
        Writer(const AEventHandler& handler,
			HttpParser&& parser,
            std::function<const IWebservModule::Srv*(const std::string&)>
            selectServer);
        virtual ~Writer();

        void	process(uint32_t events) override;
};