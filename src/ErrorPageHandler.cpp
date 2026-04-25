#include "../inc/ErrorPageHandler.hpp"

ErrorPageHandler::ErrorPageHandler( int statusCode, std::string reasonPhrase ) : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ) {}

void    ErrorPageHandler::Handler()
{
    // std::string path = getPath(); //get user defined path to error pages



    // -------- default error page --------
    
    std::ifstream       ifs( "../ErrorPages/defaultErrorPage.html" );
    std::ostringstream  oss;
    std::string         buffer;    

    oss << ifs.rdbuf();
    buffer = oss.str();

    while ( true )
    {
        size_t CodePos = buffer.find( "{{STATUS_CODE}}", 0 );
        
        if ( CodePos != std::string::npos )
        {
            std::string sCode = std::to_string( _statusCode );
            buffer.replace( CodePos, 15, sCode );
        }
        
        size_t PhrasePos = buffer.find( "{{REASON_PHRASE}}", 0 );

        if ( PhrasePos != std::string::npos )
        {
            buffer.replace( PhrasePos, 17, _reasonPhrase );
        }
        if ( CodePos == std::string::npos && PhrasePos == std::string::npos )
            break ;
    }
}


ErrorPageHandler::~ErrorPageHandler() {}

