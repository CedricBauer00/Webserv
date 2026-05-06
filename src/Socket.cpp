#include "../inc/Socket.hpp"

Socket::Socket() { std::cout << "Created Socket." << std::endl; }

Socket::~Socket() { std::cout << "Destroyed Socket." << std::endl; }


void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning
    int saved_errno = errno; // waitpid() might overwrite errno, so we save and restore it:

    while(waitpid(-1, NULL, WNOHANG) > 0); //ends all child processes - zombie processes
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    
    if (flags == -1)
        return -1;

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int Socket::createSocket( std::vector<Server> &servers )
{
    int rv, yes=1;
    struct sigaction sa;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    for ( size_t i = 0; i < servers.size(); ++i )
    {
        int _listenSockFd;

        if ((rv = getaddrinfo( servers[ i ].getDomain().c_str(), PORT, &hints, &servinfo)) != 0)
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }

        for ( p = servinfo; p != NULL; p = p->ai_next )
        {
            if ( ( _listenSockFd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 )
            {
                perror( "server: socket" );
                continue ;
            }
            if ( setsockopt( _listenSockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 )
            {
                perror( "setsockopt" );
                exit( 1 );
            }
            if ( bind( _listenSockFd, p->ai_addr, p->ai_addrlen ) == -1 )
            {
                close ( _listenSockFd );
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

        if ( listen( _listenSockFd, BACKLOG ) == -1 )
        {
            perror( "listen" );
            exit( 1 );
        }
        
        std::cout << "Listening socket created" << std::endl;

        sa.sa_handler = sigchld_handler; // reap all dead processes
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(1);
        }
        _listenFds.push_back( _listenSockFd );
    }
    return 0;
}

std::vector<int> Socket::getListenFds() { return _listenFds; }