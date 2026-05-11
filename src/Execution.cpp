#include "../inc/Execution.hpp"

Execution::Execution() {}

Execution::~Execution() {}

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    Execution::execution( std::string request, Response &Res )
{
    try
    {
        // HttpParsing( request );    
        HttpParser parser( request );

        // setStartline
        parser.setHeaders();

        Method m = parser.getMethod();
        
        serverRewrite( parser.getUri() );

        // SERVER_REWRITE
        // server{} rw

        // FIND_CONFIG
        // location{}
        // korrekten Server & Location finden anhand von HOST/PORT/URI

        // REWRITE
        // location{} rw

        // POST_REWRITE -> rewrite last / URI changed

        // PRE_ACCESS
        // limit_req

        // ACCESS
        // auth/allow

        // POST_ACCESS -> access denied -> [SEND 401/403]

        // PRE_CONTENT
        // try_files

        // CONTENT
        // proxy/static

        // LOGGING (acces-/error-log)





        // Read body
        if ( m == METHOD_POST )
            parser.setBody(); // for POST requests - last step of execution
        std::cout << ORANGE << parser.getBody() << RESET << std::endl;

        /// Response Buidling 
        // (impliziert)
        Res.setResponse( "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n<html><body>Hello, World!</body></html>" );
    }
    catch ( const HttpException& e )
    {
        ErrorPageHandler errorPage( e.getStatusCode(), e.getReasonPhrase() );
        std::cout << e.getStatusCode() << ":" << e.getReasonPhrase() << std::endl;
        errorPage.createErrorPage( Res );
        // Res.setErrorPage()
    }
}

// printf 'GET /Something HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490

void    Execution::serverRewrite( std::string uri ) //rewriting URI based on rules in config??
{
    _uri = uri;
    std::cout << "_uri = " << _uri << std::endl;

}

// Example
// server {
//     listen 3490;
//     server_name example.com;

//     # Regel 1: /old/... -> /new/...
//     rewrite ^/old/(.*)$ /new/$1 last;

//     # Regel 2: /docs -> /docs/
//     rewrite ^/docs$ /docs/ last;

//     # Regel 3: /legacy -> redirect auf /new
//     rewrite ^/legacy$ /new permanent;

//     location /new/ {
//         root /var/www/site;
//     }

//     location / {
//         root /var/www/default;
//     }
// }