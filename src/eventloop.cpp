#include "../inc/eventloop.hpp"

#define PORT "3491"
#define BACKLOG 5
#define MAX_EVENTS 10

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

// void    print_addrinfo( struct addrinfo *p, int it, char *s )
// {
//     std::cout << "\n--- iteration: " << it << "---" << std::endl;
//     std::cout << "ai_flags: " << p->ai_flags << std::endl;
//     std::cout << "ai_family: " << (p->ai_family == AF_INET ? "AF_INET (4)" : p->ai_family == AF_INET6 ? "AF_INET6 (6)" : "UNKNOWN" ) << std::endl;
//     std::cout << "ai_socktype: " << (p->ai_socktype == SOCK_STREAM ? "SOCK_STREAM" : "UNKNOWN" ) << std::endl;
//     std::cout << "ai_protocol: " << p->ai_protocol << std::endl;
//     std::cout << "ai_addrlen: " << p->ai_addrlen << std::endl;

//     if ( p->ai_canonname )
//         std::cout << "ai_canonname: " << p->ai_canonname << std::endl;

//     inet_ntop( p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof( s ) );
//     std::cout << "Address: " << s << std::endl;
// }

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

    // int it = 0;
    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        // print_addrinfo(p, it++, s);
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

    /////////

    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    int new_fd;
    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 44\r\n\r\n<html><body>Hello, World!</body></html>";
    struct epoll_event ev, events[MAX_EVENTS];

    epollfd = epoll_create1(0);
    if ( epollfd == -1 ) 
    {
        perror( "epoll_create1" );
        exit( EXIT_FAILURE );
    }
    
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if ( epoll_ctl( epollfd, EPOLL_CTL_ADD, sockfd, &ev ) == -1 )
    {
        perror( "epollwait" );
        exit( EXIT_FAILURE );
    }

<<<<<<< HEAD
=======
    while ( 1 )
    {
        nfds = epoll_wait( epollfd, events, MAX_EVENTS, -1 );
        if ( nfds == -1 )
        {
            perror( "epoll_wait" );
            exit( EXIT_FAILURE );
        }
    
        for ( int n = 0; n < nfds; ++n )// main accept() loop
        {
            // sin_size = sizeof their_addr;
            // new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
            //     &sin_size);
            // if (new_fd == -1) {
            //     perror("accept");
            //     continue;
            // }

            if ( events[ n ].data.fd == sockfd )
            {
                
            }

            //printing
            inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);
            printf("server: got connection from %s\n", s);
            //

            if (!fork()) { // this is the child process
                close(sockfd); // child doesn't need the listener
                if (send(new_fd, response, strlen(response), 0) == -1)
                    perror("send");
                close(new_fd);
                exit(0);
            }
            close(new_fd);  // parent doesn't need this
        }
    }
>>>>>>> refs/remotes/origin/execution
    return 0;
}

// wieviele connections muss epoll gleichzeitig handeln koennen? fuer den webserver

// strcut epoll_event eventQueue(MAX_INPUT_EVENTS); MAX_INPUT_EVENTS was muss das sein?

// epollwait - system call 

    //    #include <sys/epoll.h>

    //    int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);

// - waits for events on the epoll instance referred to by epfd file descriptor
// - struct epoll_event *events - buffer that returns information from the ready list about the file descriptors in the interest list that have events
// - maxevents - how many fds the function checks - must be greater than 0
// - timeout - number of milliseconds epoll_wait will block? - can we use this function if it blocks?


// EPOLLET 
// in case not all of the information is being read from the input buffer might cause epoll_wait(2) to be stuck indefinetly
// if use EPOLLET, should use non-blocking file descriptors, to avoid having a blocking read or write starve a task that is handling multiple file descriptors
// recommendation when using EPOLLET is using nonblocking file descriptors and waiting for an event only after read or write return EAGAIN

     #define MAX_EVENTS 10
           struct epoll_event ev, events[MAX_EVENTS];
           int listen_sock, conn_sock, nfds, epollfd;

           /* Code to set up listening socket, 'listen_sock',
              (socket(), bind(), listen()) omitted */

           epollfd = epoll_create1(0);
           if (epollfd == -1) {
               perror("epoll_create1");
               exit(EXIT_FAILURE);
           }

           ev.events = EPOLLIN;
           ev.data.fd = listen_sock;
           if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
               perror("epoll_ctl: listen_sock");
               exit(EXIT_FAILURE);
           }

           for (;;) {
               nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
               if (nfds == -1) {
                   perror("epoll_wait");
                   exit(EXIT_FAILURE);
               }

               for (n = 0; n < nfds; ++n)
               {
                   if (events[n].data.fd == listen_sock)
                   {
                       conn_sock = accept(listen_sock, (struct sockaddr *) &addr, &addrlen);
               
                       if (conn_sock == -1)
                       {
                           perror("accept");
                           exit(EXIT_FAILURE);
                       }

                       setnonblocking(conn_sock);
                       ev.events = EPOLLIN | EPOLLET;
                       ev.data.fd = conn_sock;
               
                       if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
                       {
                           perror("epoll_ctl: conn_sock");
                           exit(EXIT_FAILURE);
                       }
                    }
                    else
                    {
                       do_use_fd(events[n].data.fd);
                    }
                }
            }