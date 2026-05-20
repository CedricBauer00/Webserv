#include <iostream>

#include "HttpServer.hpp"
#include "Servers.hpp"
#include "../inc/Configparsing/ConfigParser.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
        return 0;
    ConfigParser confparser(argv[1]);

    try {
        confparser.parseConfig();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << confparser.getAddrToServersMap().at("0.0.0.0:80").size() << std::endl;
    std::cout << confparser.getAddrToServersMap().at("0.0.0.0:80").front() << std::endl;
    HttpServer server;
    Global AllServers;
    Server CurServ;

    CurServ.setDomain( "127.0.0.1" );
    AllServers.setServer(CurServ);
    CurServ = Server();
    CurServ.setDomain( "127.0.0.2" );
    AllServers.setServer(CurServ);
    CurServ = Server();
    CurServ.setDomain( "127.0.0.3" );
    AllServers.setServer(CurServ);

    std::vector<Server> servers = AllServers.getServers();

    if ( server.createSocket( servers ) )
    {
        std::cout << "creating socket failed." << std::endl;
        return -1;
    }
    if ( server.eventLoop() )
    {
        std::cout << "eventloop crashed." << std::endl;
        return -1;
    }
    
    return 0;
}