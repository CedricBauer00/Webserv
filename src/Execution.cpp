#include "../inc/Execution.hpp"

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    execution( std::string request )
{
    try
    {
        HttpParsing( request );    
        
        /// Vishnus Logic
        // ...

        /// Response Buidling 
    }
    catch ( const HttpException& e )
    {
        ErrorPageHandler errorPage( e.getStatusCode(), e.getReasonPhrase() );
        std::cout << e.getStatusCode() << ":" << e.getReasonPhrase() << std::endl;
        errorPage.createErrorPage();
    }
}