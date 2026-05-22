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
		Epoller* const										_epoller;

		int		_setNonBlocking(int fd);

    public:
		AEventHandler() = delete;
        AEventHandler(const int fd,
			const std::vector<const IWebservModule::SrvNode*>& servers,
			Epoller* const epoller);
        virtual ~AEventHandler();

		const std::vector<const IWebservModule::SrvNode*>&	getServers() const;
		Epoller* const	getEpoller() const;
        const int		getFd() const;
        void*			getInAddr(struct sockaddr *sa) const;
        virtual void	process() = 0;
};
