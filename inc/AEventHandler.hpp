#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <functional>
#include "Configparsing/ConfigParser.hpp"
#include "Configparsing/WebservCoreModule.hpp"
#include "Epoller.hpp"
#include "HttpParser.hpp"
#include "Response.hpp"

class AEventHandler {
	public:
		class	wouldBlockException: public std::exception {
			public:
				const char* what() const throw() {
					return "File operation would block";
				}
		};

	protected:
		static int	_dupFd(int fd);
		static void	_setNonBlocking(int fd);

	protected:
		WebservSocket	_sock;
		uint32_t		_events;

		void		_modifyEvent(const uint32_t events);
        void    	_printSocketError();

    public:
		const Epoller&											epoller;
		const std::function<const Srv*(const std::string&)>&	selectSrv;

		AEventHandler() = delete;
        AEventHandler(WebservSocket&& sock,
			const uint32_t events,
			const Epoller& e,
			const std::function<const Srv*(const std::string&)>& selectServer);
		AEventHandler(AEventHandler&& other) noexcept;
        virtual ~AEventHandler();

		uint32_t		getEvents() const;
		int				getFd() const;
        void*			getInAddr(struct sockaddr_storage& st) const;
		in_port_t		getPort(struct sockaddr_storage& st) const;
        virtual void	process(uint32_t events) = 0;
};
