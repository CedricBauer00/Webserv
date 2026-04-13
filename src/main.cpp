#include "eventloop.hpp"


int main( void )
{
    httpServer server;

    if ( !server.createSocket() )
        return -1;
    
    if ( server.eventLoop() )
        return -1;
        
    return 0;
}