#include "eventloop.hpp"


int main( void )
{
    httpServer server;

    if ( server.createSocket()  )
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