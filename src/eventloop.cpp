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

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void    print_addrinfo( struct addrinfo *p, int it, char *s )
{
    std::cout << "\n--- iteration: " << it << "---" << std::endl;
    std::cout << "ai_flags: " << p->ai_flags << std::endl;
    std::cout << "ai_family: " << (p->ai_family == AF_INET ? "AF_INET (4)" : p->ai_family == AF_INET6 ? "AF_INET6 (6)" : "UNKNOWN" ) << std::endl;
    std::cout << "ai_socktype: " << (p->ai_socktype == SOCK_STREAM ? "SOCK_STREAM" : "UNKNOWN" ) << std::endl;
    std::cout << "ai_protocol: " << p->ai_protocol << std::endl;
    std::cout << "ai_addrlen: " << p->ai_addrlen << std::endl;

    if ( p->ai_canonname )
        std::cout << "ai_canonname: " << p->ai_canonname << std::endl;

    inet_ntop( p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof( s ) );
    std::cout << "Address: " << s << std::endl;
}

int httpServer::run()
{
    int rv, sockfd, yes=1;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];

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

    int it = 0;
    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        
        print_addrinfo(p, it++, s);
        if ( ( sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 )
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

        std::cout << "printing family: " << (p->ai_family == AF_INET ? "AF_INET" : "AF_INET6") << std::endl;

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

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    std::cout << "Server wating for connection...\n" << std::endl;

    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    int new_fd;
    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 44\r\n\r\n<html><body>Hello, World!</body></html>";

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
            &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, response, strlen(response), 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0; 

    return 0;
}