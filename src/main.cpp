#include <iostream>

#include "../inc/Configparsing/ConfigParser.hpp"
#include "../inc/WebServ.hpp"

int main(int argc, char* argv[])
{
    // HttpServer server;
    // Global AllServers;
    // Server CurServ;

    // CurServ.setDomain( "127.0.0.1" );
    // AllServers.setServer(CurServ);
    // CurServ = Server();
    // CurServ.setDomain( "127.0.0.2" );
    // AllServers.setServer(CurServ);
    // CurServ = Server();
    // CurServ.setDomain( "127.0.0.3" );
    // AllServers.setServer(CurServ);

    // std::vector<Server> servers = AllServers.getServers();

    // if ( server.createSocket( servers ) )
    // {
    //     std::cout << "creating socket failed." << std::endl;
    //     return -1;
    // }
    // if ( server.eventLoop() )
    // {
    //     std::cout << "eventloop crashed." << std::endl;
    //     return -1;
    // }
    
    if (argc != 2)
        return 0;
    // ConfigParser confparser(argv[1]);

    try {
        // confparser.parseConfig();
		WebServ webserv(argv[1]);
		webserv.run();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    // Socket      socket;
    // HttpServer  server;
    // Global      AllServers;
    // Server      CurServ;

    // CurServ.setDomain( "127.0.0.1" );
    // CurServ.setPort( 8081 );
    // CurServ.setServerName( "example.com" );

    // AllServers.setServer(CurServ);
    // CurServ = Server();
    // CurServ.setDomain( "127.0.0.2" );
    // CurServ.setPort( 8082 );
    // CurServ.setServerName( "localhost" );


    // AllServers.setServer(CurServ);
    // CurServ = Server();
    // CurServ.setDomain( "127.0.0.3" );
    // CurServ.setPort( 8083 );
    // CurServ.setServerName( "api.example.com" );


    // AllServers.setServer(CurServ);

    // std::vector<Server> servers = AllServers.getServers();

    // if ( socket.createSocket( servers ) )
    // {
    //     std::cout << "creating socket failed." << std::endl;
    //     return -1;
    // }
    // if ( server.eventLoop( socket.getListenFds(), servers ) )
    // {
    //     std::cout << "eventloop crashed." << std::endl;
    //     return -1;
    // }
    
    return 0;
}