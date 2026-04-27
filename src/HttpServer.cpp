#include <cerrno>
#include <cstdlib>
#include "../inc/HttpServer.hpp"
#include "../inc/HttpException.hpp"
#include "../inc/Exceptions.hpp"
#include "../inc/ErrorPageHandler.hpp"

#define PORT "3490"
#define BACKLOG 5
#define MAX_EVENTS 10

HttpServer::HttpServer() : _listenFds()
{
    std::cout << "Server created" << std::endl;
}

HttpServer::~HttpServer()
{
    std::cout << "Server destroyed" << std::endl;
}

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

int HttpServer::createSocket( std::vector<Server> &servers )
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

void HttpServer::closeEvent(struct epoll_event &ev, int epollfd, int &fdCount)
{
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, static_cast<Client*>(ev.data.ptr)->getFd(), NULL) == -1) //HTTP 1.0 doesnt add FD with event to epollfds, so we dont have to remove
        std::cerr << "EPOLL_CTL_DEL_ERROR: " << strerror(errno) << '\n';
    if (close(static_cast<Client*>(ev.data.ptr)->getFd()) == -1) //delete the eventHandler pointer;
        std::cerr << "CLOSE_ERROR: " << strerror(errno) << '\n';
    else
        std::cout << RED << "Closed: fd=" << static_cast<Client*>(ev.data.ptr)->getFd() << RESET << std::endl;
    delete static_cast<Client*>(ev.data.ptr);
    --fdCount;
}

int HttpServer::eventLoop()
{
    socklen_t addrlen;
    struct sockaddr_storage clientAddr;
    char s[INET6_ADDRSTRLEN];
    int new_fd, epollfd, nfds, ret, fdCount = 0;
    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<html><body>Hello, World!</body></html>";
    struct epoll_event ev, events[MAX_EVENTS];

    epollfd = epoll_create1(O_CLOEXEC);

    if ( epollfd == -1 )
    {
        perror( "epoll_create1" );
        exit( EXIT_FAILURE );
    }
    
    for ( int i : _listenFds )
    {
        if (set_nonblocking( i ) == -1)
        {
            perror("set_nonblocking");
            exit(EXIT_FAILURE);
        }
        
        ev = {.events = EPOLLIN, .data = {.ptr =  new ListenHandler(i) }};
        
        if ( epoll_ctl( epollfd, EPOLL_CTL_ADD, i , &ev ) == -1 )
        {
            perror( "epollwait(listening)" );
            exit( EXIT_FAILURE );
        }
        ++fdCount;
    
    }

    std::cout << "Running webserver" << std::endl;
    while (1)
    {
        printf("Number of open fds: %d\n", fdCount);
        
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            perror( "epoll_wait" );
            if (errno == EINTR)
                continue;
        
            exit( EXIT_FAILURE );
        }

    
        for (int n = 0; n < nfds; ++n)
        {
            if ( std::find( _listenFds.begin(), _listenFds.end(), static_cast<EventHandler*>(events[n].data.ptr)->getFd() ) != _listenFds.end() )
            {
                addrlen = sizeof clientAddr;
                new_fd = accept( static_cast<EventHandler*>(events[n].data.ptr)->getFd(), (struct sockaddr*)&clientAddr, &addrlen);
            
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                
                std::cout << GREEN << "new_fd = " << RESET << new_fd << std::endl;

                //printing
                inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr *)&clientAddr), s, sizeof s);
                printf("server: accepted connection from %s port %d\n",
                    s, ntohs(((struct sockaddr_in *)&clientAddr)->sin_port));
                
                try
                {
                    if (set_nonblocking(new_fd) == -1)
                        throw std::runtime_error("SET_NONBLOCKING_ERROR");
                    
                    ev.data.ptr = new Client(new_fd);
                    ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
                    
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev) == -1)
                    {
                        delete static_cast<Client*>(ev.data.ptr); 
                        throw std::runtime_error("EPOLL_CTL_ERROR");
                    } // HTTP 1.0 doesnt add incoming FD to epoll
                    
                    ++fdCount;
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    if (close(new_fd) == -1)
                        std::cerr << "CLOSE_ERROR: " << strerror(errno) << '\n';
                }
                printf("--------accepted--------\n\n");
            }   
            else
            {
                printf("server: got event on fd %d of type %u\n",
                    static_cast<Client*>(events[n].data.ptr)->getFd(), events[n].events);
                
                if (events[n].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
                {
                    printf("Client disconnected: fd=%d\n",
                        static_cast<Client*>(events[n].data.ptr)->getFd());
                    closeEvent(events[n], epollfd, fdCount);
                
                    continue;
                }

                Client &client = *(static_cast<Client*>(events[n].data.ptr));
                
                if (events[n].events & EPOLLIN)
                {
                    ret = client.receiveFromClient();
                    if (ret < 1)
                    {
                        if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                            (void)1;
                        else
                        {
                            closeEvent(events[n], epollfd, fdCount);
                            continue;
                        }
                    }
                }
                std::cout << client.getComplHeader() << std::endl;
                std::cout << "Received:\n" << client.getRequest() << std::endl;
                if ( client.getComplHeader() )
                {
                    HttpParser result;
                    
                    try
                    {
                        result.setHeaders(  client.getRequest() );

                    }
                    catch ( const HttpException& e )
                    {
                        ErrorPageHandler errorPage( e.getStatusCode(), e.getReasonPhrase() );
                        std::cout << e.getStatusCode() << ":" << e.getReasonPhrase() << std::endl;
                        errorPage.createErrorPage();
                    }

                }

                if (events[n].events & EPOLLIN || ((events[n].events & EPOLLOUT) && client.getSendPos()))
                {
                    if (client.sendToClient(response) == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            continue;
                    }
                    closeEvent(events[n], epollfd, fdCount);
                }
                printf("---------------\n\n");
            }
        }
    }
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
