#include "../inc/HttpServer.hpp"
#include "../inc/HttpParser.hpp"
#include "../inc/Client.hpp"
#include <cerrno>
#include <cstdlib>
#include <map>

#define PORT "3490"
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

HttpServer::HttpServer() : _listenSock( 0 ) {
    std::cout << "Server created" << std::endl;
}

HttpServer::~HttpServer() {
    std::cout << "Server destroyed" << std::endl;
}

int HttpServer::get_sock() {
    return _listenSock;
}

void sigchld_handler(int s) {
    (void)s; // quiet unused variable warning
    int saved_errno = errno; // waitpid() might overwrite errno, so we save and restore it:

    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int HttpServer::createSocket() {
    int rv, yes=1;
    struct sigaction sa;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for ( p = servinfo; p != NULL; p = p->ai_next ) {
        if ( ( _listenSock = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 ) {
            perror( "server: socket" );
            continue ;
        }
        if ( setsockopt( _listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) {
            perror( "setsockopt" );
            exit( 1 );
        }
        if ( bind( _listenSock, p->ai_addr, p->ai_addrlen ) == -1 ) {
            close ( _listenSock );
            perror( "server: bind" );
            continue ;
        }
        break ;
    }

    freeaddrinfo( servinfo );

    if ( p == NULL ) {
        fprintf( stderr, "server: failed to bind\n" );
        exit( 1 );
    }

    if ( listen( _listenSock, BACKLOG ) == -1 ) {
        perror( "listen" );
        exit( 1 );
    }
    std::cout << "Listening socket created" << std::endl;

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    return 0;
}

void HttpServer::closeEvent(int fd, int epollfd, int &fdCount) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        perror("epoll_ctl:delete fd");
        throw std::runtime_error("EPOLL_CTL_ERROR");
    }
    if (close(fd) == -1)
        throw std::runtime_error("CLOSE_ERROR");
    std::cout << RED << "Closed: fd=" << fd << RESET << std::endl;
    --fdCount;
}

int HttpServer::eventLoop() {
    socklen_t addrlen;
    struct sockaddr_storage clientAddr;
    char s[INET6_ADDRSTRLEN];
    int new_fd, epollfd, nfds, ret, fdCount = 0;
    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<html><body>Hello, World!</body></html>";
    // const char* response =
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/html; charset=utf-8\r\n"
    //     "Content-Length: 44\r\n"
    //     "\r\n"
    //     "<html><body>Hello, World!</body></html>";
    struct epoll_event ev, events[MAX_EVENTS];
    std::map<int, Client> clients;

    epollfd = epoll_create1(O_CLOEXEC);
    if ( epollfd == -1 ) {
        perror( "epoll_create1" );
        exit( EXIT_FAILURE );
    }
    
    if (set_nonblocking(_listenSock) == -1) {
        perror("set_nonblocking");
        exit(EXIT_FAILURE);
    }
    ev = {.events = EPOLLIN, .data = {.fd = _listenSock}};
    if ( epoll_ctl( epollfd, EPOLL_CTL_ADD, _listenSock, &ev ) == -1 ) {
        perror( "epollwait(listening)" );
        exit( EXIT_FAILURE );
    }
    ++fdCount;

    std::cout << "Running webserver" << std::endl;
    while ( 1 ) {
        printf("Number of open fds: %d\n", fdCount);
        nfds = epoll_wait( epollfd, events, MAX_EVENTS, -1 );
        if ( nfds == -1 ) {
            perror( "epoll_wait" );
            exit( EXIT_FAILURE );
        }
    
        for ( int n = 0; n < nfds; ++n ) {
            if ( events[ n ].data.fd == _listenSock ) {
                addrlen = sizeof clientAddr;
                new_fd = accept(_listenSock, (struct sockaddr*)&clientAddr, &addrlen);
                if (new_fd == -1) {
                    perror("accept");
                    exit( EXIT_FAILURE ); // TODO: server should never crash (rlimit < no. of fds)
                }
                std::cout << GREEN << "new_fd = " << RESET << new_fd << std::endl;

                //printing
                inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr *)&clientAddr), s, sizeof s);
                printf("server: accepted connection from %s port %d\n",
                    s, ntohs(((struct sockaddr_in *)&clientAddr)->sin_port));
                
                if (set_nonblocking(new_fd) == -1) {
                    perror("set_nonblocking"); 
                    exit(EXIT_FAILURE); // TODO: server should never crash (rlimit < no. of fds)
                }

                ev = {.events = EPOLLIN | EPOLLRDHUP | EPOLLOUT | EPOLLET, .data = {.fd = new_fd}};
                if ( epoll_ctl( epollfd, EPOLL_CTL_ADD, new_fd, &ev ) == -1 ) {
                    perror( "epoll_ctl:P new_fd" );
                    exit( EXIT_FAILURE ); // TODO: server should never crash (rlimit < no. of fds)
                }
                clients[new_fd] = Client();
                ++fdCount;
                printf("---------------\n\n");
            }   
            else {
                printf("server: got event on fd %d of type %u\n", events[n].data.fd, events[n].events);
                if (events[n].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
                    printf("Client disconnected: fd=%d\n", events[n].data.fd);
                    closeEvent(events[n].data.fd, epollfd, fdCount);
                    clients.erase(events[n].data.fd);
                    continue;
                }

                Client &client = clients[events[n].data.fd];
                if (events[n].events & EPOLLIN) {
                    ret = client.receiveFromClient(events[n].data.fd);
                    if (ret < 1) {
                        if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                            (void)1;
                        else {
                            closeEvent(events[n].data.fd, epollfd, fdCount);
                            clients.erase(events[n].data.fd);
                            continue;
                        }
                    }
                    printf("Received: %s\n", client.getRequest().c_str());
                }

                // HttpParser result( client.getRequest() );
                // if ( result.parse() == -1 )
                //     return -1;

                if (events[n].events & EPOLLIN || (events[n].events & EPOLLOUT && client.getSendPos())) {
                    if (client.sendToClient(events[n].data.fd, response) == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            continue;
                    }
                    closeEvent(events[n].data.fd, epollfd, fdCount);
                    clients.erase(events[n].data.fd);
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

// response codes function

// void Response::setStatus(int code)
// {
//     static const std::map<int, std::string> reasons = {
//         {200, "OK"},
//         {201, "Created"},
//         {204, "No Content"},
//         {301, "Moved Permanently"},
//         {308, "Permanent Redirect"},
//         {400, "Bad Request"},
//         {401, "Unauthorized"},
//         {402, "Payment Required"},
//         {403, "Forbidden"},
//         {404, "Not Found"},
//         {405, "Method Not Allowed"},
//         {406, "Not Acceptable"},
//         {407, "Proxy Authentication Required"},
//         {408, "Request Timeout"},
//         {409, "Conflict"},
//         {410, "Gone"},
//         {411, "Length Required"},
//         {412, "Precondition Failed"},
//         {413, "Payload Too Large"},
//         {414, "URI Too Long"},
//         {415, "Unsupported Media Type"},
//         {416, "Range Not Satisfiable"},
//         {417, "Expectation Failed"},
//         {418, "I'm a teapot"},
//         {421, "Misdirected Request"},
//         {422, "Unprocessable Entity"},
//         {423, "Locked"},
//         {424, "Failed Dependency"},
//         {425, "Too Early"},
//         {426, "Upgrade Required"},
//         {428, "Precondition Required"},
//         {429, "Too Many Requests"},
//         {431, "Request Header Fields Too Large"},
//         {451, "Unavailable For Legal Reasons"},
//         {500, "Internal Server Error"},
//         {501, "Not Implemented"},
//         {502, "Bad Gateway"},
//         {503, "Service Unavailable"},
//         {504, "Gateway Timeout"},
//         {505, "HTTP Version Not Supported"},
//         {506, "Variant Also Negotiates"},
//         {507, "Insufficient Storage"},
//         {508, "Loop Detected"},
//         {510, "Not Extended"},
//         {511, "Network Authentication Required"}
//     };
//     _statusCode = code;
//     if (reasons.count(code))
//         _reasonPhrase = reasons.at(code);
//     else
//         _reasonPhrase = "Unknown";
// }
