#include "eventloop.hpp"

#define PORT "3490"
#define BACKLOG 5
//  getaddrinfo() 

// socket()

// setsockoption()

// bind()

// if ( HTTP 1.0 )
// {
// listen()

// accept()

// recv()

// send()

// close()
// }

// else if ( HTTP 1.1 )
// {
// listen()

// accept()

// recv()

// send()

// close()

// }

httpServer::httpServer()
{
    std::cout << "Server created" << std::endl;
}

httpServer::~httpServer()
{
    std::cout << "Server destroyed" << std::endl;
}

int httpServer::run()
{
    int rv, sockfd, yes=1;

    std::cout << "Running webserver" << std::endl;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        if ( ( sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol ) ) != -1 )
        {
            perror( "server: socket" );
            continue ;
        }

        if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 )
        {
            perror( "setsockopt" );
            exit( 1 );
        }

        if ( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 )
        {
            close ( sockfd );
            perror( "server: bind" );
            continue ;
        }
        break ;
    }

    freeaddrinfo( servinfo );

    if ( p == NULL )
    {
        fprintf( stderr, "server: failed to bind\n" );
        exit( 1 );
    }

    if ( listen( sockfd, BACKLOG ) == -1 )
    {
        perror( "listen" );
        exit( 1 );
    }



    return 0;
}