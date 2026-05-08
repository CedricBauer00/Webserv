#include "../inc/Execution.hpp"

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    execution( std::string request, Response &Res )
{
    try
    {
        // HttpParsing( request );    
        HttpParser result( request );

        // setStartline
        result.setHeaders();
        
        // SERVER_REWRITE
        // server{} rw

        // FIND_CONFIG
        // location{}

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

        // LOG





        /// Response Buidling 
        // (impliziert)

        // Read body 
        result.setBody(); // for POST requests - last step of execution
        std::cout << ORANGE << result.getBody() << RESET << std::endl;
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