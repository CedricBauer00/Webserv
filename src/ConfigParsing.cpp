#include "../inc/ConfigParsing.hpp"

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

void    Server::setPort( size_t port )
{
    _port = port;
}

void    Server::setServerName( std::string sName )
{
    _serverName = sName;
}

std::string Server::getServerName()
{
    return _serverName;
}

std::string Server::getDomain()
{
    return _domain;
}

size_t Server::getPort()
{
    return _port;
}

Location::Location() : _root("") {}

Location::~Location() {}

std::string Location::getRoot()
{
    return _root;
}

