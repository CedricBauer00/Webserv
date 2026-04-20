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
        unsigned int _port;
        std::vector<Location> locations;
    public:
        Server();
        ~Server();
        std::string getDomain();
        unsigned int getPort();
        void setDomain( std::string domain );
};

class Global
{
    private:
        std::vector<Server> _servers;
    public:
        Global();
        ~Global();
        unsigned int getDomainNum();
        std::vector<Server> getServers();
        void setServer( Server s );
};
