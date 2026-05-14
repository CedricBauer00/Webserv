#include "HttpServer.hpp"
#include "../inc/ConfigParsing.hpp"
#include "../inc/Socket.hpp"

int main( void )
{
    Socket      socket;
    HttpServer  server;
    Global      AllServers;
    Server      CurServ;

    CurServ.setDomain( "127.0.0.1" );
    CurServ.setPort( 8081 );
    CurServ.setServerName( "example.com" );

    AllServers.setServer(CurServ);
    CurServ = Server();
    CurServ.setDomain( "127.0.0.2" );
    CurServ.setPort( 8082 );
    CurServ.setServerName( "localhost" );


    AllServers.setServer(CurServ);
    CurServ = Server();
    CurServ.setDomain( "127.0.0.3" );
    CurServ.setPort( 8083 );
    CurServ.setServerName( "api.example.com" );


    AllServers.setServer(CurServ);

    std::vector<Server> servers = AllServers.getServers();

    if ( socket.createSocket( servers ) )
    {
        std::cout << "creating socket failed." << std::endl;
        return -1;
    }
    if ( server.eventLoop( socket.getListenFds(), servers ) )
    {
        std::cout << "eventloop crashed." << std::endl;
        return -1;
    }
    
    return 0;
}