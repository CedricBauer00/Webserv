#include "eventloop.hpp"


int main( void )
{
    httpServer server;

    server.createSocket();
    server.eventLoop();
    
    return 0;
}