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

class AEventHandler {
	protected:
		int								_fd;
		const std::vector<const Srv*>&	_servers;
		const Epoller&					_epoller;

        static int	_dupFd(int fd);
		void		_setNonBlocking(int fd);
        void    	_printSocketError();
        std::function<const Srv*(const std::string&)>
		_selectServerFactory();

    public:
		class	wouldBlockException: public std::exception {
			public:
				const char* what() const throw() {
					return "File operation would block";
				}
		};

		AEventHandler() = delete;
        AEventHandler(int fd,
			const std::vector<const Srv*>& servers,
			const Epoller& epoller,
			const uint32_t events,
			const Epoller::EpollOperation op);
        virtual ~AEventHandler();

		const std::vector<const Srv*>&	getServers() const;
		const Epoller&	getEpoller() const;
		int				getFd() const;
        void*			getInAddr(struct sockaddr_storage& st) const;
		in_port_t		getPort(struct sockaddr_storage& st) const;
		void			closeFd();
        void            setFd(int fd);
        virtual void	process(uint32_t events) = 0;
};
