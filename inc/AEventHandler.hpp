#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Configparsing/ConfigParser.hpp"

class Epoller;

class AEventHandler {
	protected:
		const int											_fd;
		const std::vector<const IWebservModule::SrvNode*>&	_servers;
		const Epoller* const								_epoller;

		int		_setNonBlocking(int fd);

    public:
		AEventHandler() = delete;
        AEventHandler(const int fd,
			const std::vector<const IWebservModule::SrvNode*>& servers,
			const Epoller* const epoller);
        virtual ~AEventHandler();

		const std::vector<const IWebservModule::SrvNode*>&	getServers() const;
		const Epoller* 	getEpoller() const;
        const int		getFd() const;
		void			closeFd();
        void*			getInAddr(struct sockaddr *sa) const;
        void			addSelfToEpoll(uint32_t events) const;
		void			delSelfFromEpoll() const;
        virtual void	process(uint32_t events) = 0;
};
