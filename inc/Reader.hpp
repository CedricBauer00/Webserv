#pragma once

#include <functional>
#include "Listener.hpp"
#include "Response.hpp"

class Reader: public AEventHandler {
	private:
		const size_t	        		BUFFER_SIZE{4096};
		std::string		        		_request;
		bool			        		_complHeader{false};

		int			_acceptConn(int listenFd);
		void		_receiveFromClient();
        std::function<const IWebservModule::Srv*(const std::string&)>
		_selectServerFactory();
	public:
		Reader() = delete;
		Reader(const Listener& listener);
		virtual ~Reader();

		const std::string&	getRequest() const;
		bool				getcomplHeader() const;
		void				process(uint32_t events) override;
};