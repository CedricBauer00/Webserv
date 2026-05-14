#pragma once
#include <vector>
#include <string>

class Location
{
    private:
        std::string _root;
    public:
        Location();
        ~Location();
        std::string getRoot();
};

class Server
{
    private:
        std::string _domain;
        int         _port;
        std::vector<Location> _locations;
    public:
        Server();
        ~Server();
        void    setDomain( std::string domain );
        void    setPort( int port );
        std::string getDomain();
};

class Global
{
    private:
        std::vector<Server> _servers;
    public:
        Global();
        ~Global();
        void    setServer( Server s );
        std::vector<Server> getServers();
};