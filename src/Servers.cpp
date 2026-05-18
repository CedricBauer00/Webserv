#include "Servers.hpp"

Global::Global() : _servers()
{

}

Global::~Global()
{

}

void    Global::setServer( Server s )
{
    _servers.push_back( s );
}  

std::vector<Server> Global::getServers()
{
    return _servers;
}

Server::Server() :_domain(""), _port(0)
{

}

Server::~Server()
{

}

void    Server::setDomain( std::string domain )
{
    _domain = domain;
}

void    Server::setPort( int port )
{
    _port = port;
}

std::string Server::getDomain()
{
    return _domain;
}

int Server::getPort()
{
    return _port;
}

Location::Location() : _root("")
{
    
}

Location::~Location()
{

}

std::string Location::getRoot()
{
    return _root;
}
