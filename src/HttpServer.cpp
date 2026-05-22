#include <cerrno>
#include <cstdlib>
#include "../inc/HttpServer.hpp"
#include "../inc/Socket.hpp"
#include "../inc/Execution.hpp"

#define MAX_EVENTS 10

HttpServer::HttpServer()
{
    std::cout << "Server created" << std::endl;
}

HttpServer::~HttpServer()
{
    std::cout << "Server destroyed" << std::endl;
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

int HttpServer::eventLoop( std::vector<int> listenFds, std::vector<Server> servers )
{
    socklen_t               addrlen;
    struct sockaddr_storage clientAddr;
    char                    s[INET6_ADDRSTRLEN];
    int                     new_fd, epollfd, nfds, ret, fdCount = 0;
    struct epoll_event      ev, events[MAX_EVENTS];
  
    Response                res;

    epollfd = epoll_create1(O_CLOEXEC);

    if ( epollfd == -1 )
    {
        perror( "epoll_create1" );
        exit( EXIT_FAILURE );
    }
    
    for ( int i : listenFds )
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
        // check client disconnecting time
        if (nfds == -1)
        {
            perror( "epoll_wait" );
            if (errno == EINTR)
                continue;
        
            exit( EXIT_FAILURE );
        }

        for (int n = 0; n < nfds; ++n)
        {
            if ( std::find( listenFds.begin(), listenFds.end(), static_cast<EventHandler*>( events[ n ].data.ptr )->getFd() ) != listenFds.end() )
            {
                addrlen = sizeof clientAddr;
                new_fd = accept( static_cast<EventHandler*>( events[ n ].data.ptr )->getFd(), ( struct sockaddr* )&clientAddr, &addrlen );
            
                if ( new_fd == -1 )
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
                    
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev) == -1) // get timestamp for slowloris check
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
                
                if (events[n].events & EPOLLIN) // update timestamp for bytes read
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

                // const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<html><body>Hello, World!</body></html>";

                if ( client.getComplHeader() ) // if false && now - lastActivity > header_timeout -> close connection
                {                    
                    Execution exec;
                    exec.execution( client.getRequest(), res, servers );
                }
                if (events[n].events & EPOLLIN || ((events[n].events & EPOLLOUT) && client.getSendPos()))
                {
                    if (client.sendToClient( res.getResponse() ) == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            continue;
                    }
                    closeEvent(events[n], epollfd, fdCount);
                }
                printf("---------------\n\n");
                res.clear();
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
