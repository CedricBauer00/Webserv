#pragma once

#include "Listener.hpp"
#include "Response.hpp"

class Reader: public AEventHandler {
	private:
		const size_t	        		BUFFER_SIZE{4096};
		std::string		        		_request;
		bool			        		_complHeader{false};
        const struct sockaddr_storage	_clientSockAddr;

		int		    _receiveFromClient();
        Response    _buildResponse() const;
        void    	_createWriter();

	public:
		Reader() = delete;
		Reader(const int fd, struct sockaddr_storage& sockAddr,
            const Listener& listener);
		virtual ~Reader();

		Response	getResponse() const;
		void		process(uint32_t events) override;
};