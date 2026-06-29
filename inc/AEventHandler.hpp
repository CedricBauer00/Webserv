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

		static std::function<const Srv*(const std::string&)>
		selectServerFactory(const std::vector<const Srv*>& srvs);

	protected:
		static int	_dupFd(int fd);
		static void	_setNonBlocking(int fd);

	protected:
		WebservSocket									_sock;
		uint32_t										_events;
		const Epoller&									_epoller;
		std::function<const Srv*(const std::string&)>	_selectServer;

		void		_modifyEvent(const uint32_t events);
        void    	_printSocketError();

    public:
		AEventHandler() = delete;
        AEventHandler(WebservSocket&& sock,
			const uint32_t events,
			const Epoller& epoller,
			std::function<const Srv*(const std::string&)>&& selectServer);
		AEventHandler(AEventHandler&& other) noexcept;
        virtual ~AEventHandler();

		const std::vector<const Srv*>&	getServers() const;
		const Epoller&	getEpoller() const;
		int				getFd() const;
        void*			getInAddr(struct sockaddr_storage& st) const;
		in_port_t		getPort(struct sockaddr_storage& st) const;
		void			closeFd();
        virtual void	process(uint32_t events) = 0;
};
