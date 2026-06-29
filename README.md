This project has been created as part of the 42 curriculum by <vramacha>, <cbauer>.

# Webserv

## Description
This project is an implementation of a non-blocking HTTP/1.0 server implemented in C++. The primary goal of this project is to create the architechture of a webserver from scratch, focusing on efficient resource management and concurreent connection handling.

The server utilizes an **event-driven architechture** powered by `epoll` (on Linux), allowing it to handle multiple simultaneous client connections without the overhead of multi-threading. It implements a non-blocking I/O model to ensure that the server remains responsive even under heavy load.

### Key Features:
- **Event-Driven Core**: Uses `epoll` for efficient event monitoring and handling.
- **HTTP/1.1 Support**: Implements standard methods including `GET`, `POST`, and `DELETE`.
- **CGI Integration**: Supports Common Gateway Interface (CGI)to execute external scripts (e.g. `.py`, `.sh`).
- **Dynamic Configuration**: Server behavior, ports, and server names are defined via a `.config` file.
- **Error Handling**: Comprehansive handling of HTTP status codes (404 Not Found, 403 Forbidden, 500 Internal Server Error, etc.).
- **Visualization UI**: Includes a frontend interface to visualize server activitiy and request processing.

## Instructions

### Compilation
To compile the project, ensure you are in the root directory and run:
```bash
make
```

### Execution
Start the server by running the executable and passing the configuration file as an argument:

```
./webserv websev.conf
```

### Accessing the Server 
Once the server is running, you can interact with it in two ways:

1. **Via Browser**: Enter the server's IP and port (e.g. http://localhost::8080) to access the hosted pages and the visualization UI.'
2. **Via Terminal**: Use tools like curl or telnet to send raw HTTP requests:
```
curl -v http://localhost:8080/index.html
```

## Resources

### References 

## AI Usage
AI was used as a technical assistent during the developement process for the following tasks:
- **Research**: Gathering neccessary knowledge for the implementation of the project. Engaging in educational dialog for deepening understanding of different concepts.
- **Architectural Guidance**: Refining the event-driven loop.
- **Debugging**: Analyzing and resolving bugs and erros throughout the project.
