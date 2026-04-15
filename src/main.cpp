#include "Eventloop.hpp"


int main( void )
{
    HttpServer server;

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