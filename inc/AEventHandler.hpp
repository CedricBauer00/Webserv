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

class Epoller;

class AEventHandler {
	protected:
		const int											_fd;
		const std::vector<const IWebservModule::Srv*>&		_servers;
		const Epoller&										_epoller;

        static int		_dupFd(int fd);

		void			_setNonBlocking(int fd);
        void    		_printSocketError();
        std::function<const IWebservModule::Srv*(const std::string&)>
		_selectServerFactory();

    public:
		class	wouldBlockException: public std::exception {
			public:
				const char* what() const throw() {
					return "File operation would block";
				}
		};

		AEventHandler() = delete;
        AEventHandler(const int fd,
			const std::vector<const IWebservModule::Srv*>& servers,
			const Epoller& epoller,
			const uint32_t events);
        virtual ~AEventHandler();

		const std::vector<const IWebservModule::Srv*>&	getServers() const;
		int				getFd() const;
		const Epoller&	getEpoller() const;
		void			closeFd(int fd);
        void*			getInAddr(struct sockaddr_storage& st) const;
		in_port_t		getPort(struct sockaddr_storage& st) const;
        virtual void	process(uint32_t events) = 0;
};
