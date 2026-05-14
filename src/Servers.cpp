#include "Servers.hpp"

Global::Global() : _servers() {}

Global::~Global() {}

void    Global::setServer( Server s )
{
    _servers.push_back( s );
}  

std::vector<Server> Global::getServers()
{
    return _servers;
}

Server::Server() :_domain(""), _port(0) {}

Server::~Server() {}

void    Server::setDomain( std::string domain )
{
    _domain = domain;
}

<<<<<<< HEAD:src/Configparsing/ConfigParsing.cpp
void    Server::setPort( size_t port )
=======
void    Server::setPort( int port )
>>>>>>> f264d76 (FIX[configParsing]: push_back on deque):src/Servers.cpp
{
    _port = port;
}

<<<<<<< HEAD:src/Configparsing/ConfigParsing.cpp
void    Server::setServerName( std::string sName )
{
    _serverName = sName;
}

std::string Server::getServerName()
{
    return _serverName;
}

=======
>>>>>>> f264d76 (FIX[configParsing]: push_back on deque):src/Servers.cpp
std::string Server::getDomain()
{
    return _domain;
}

size_t Server::getPort()
{
<<<<<<< HEAD:src/Configparsing/ConfigParsing.cpp
    return _port;
=======
    
>>>>>>> f264d76 (FIX[configParsing]: push_back on deque):src/Servers.cpp
}

Location::Location() : _root("") {}

Location::~Location() {}

std::string Location::getRoot()
{
    return _root;
}
