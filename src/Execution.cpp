#include "../inc/Execution.hpp"

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    execution( std::string request, Response &Res )
{
    try
    {
        HttpParsing( request );    
        
        /// Vishnus Logic
        // ...

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