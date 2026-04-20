#pragma once

#include <vector>
#include <string>

// ADDED: rewrite rule model for server-level rules
struct RewriteRule
{
    std::string pattern;
    std::string replacement;
    bool        redirect;
    int         code;
};

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
        std::string _serverName;
        
        // ADDED: server-level rewrite rules
        std::vector<RewriteRule> _rewriteRules;
    public:
        Server();
        ~Server();
        std::string getDomain();
        size_t getPort();
        void setDomain( std::string domain );
        void    setPort( size_t port );
        void    setServerName( std::string sName );
        std::string getServerName();


        // ADDED: rewrite rules accessors
        std::vector<RewriteRule> getRewriteRules();
        void setRewriteRule( RewriteRule rule );
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
