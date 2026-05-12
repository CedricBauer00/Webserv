#include "../inc/PageHandler.hpp"

PageHandler::PageHandler( int statusCode, std::string reasonPhrase ) : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ) {}

PageHandler::~PageHandler() {}

void    PageHandler::setErrorPage( Response &Res )
{
    std::string path = "pages/defaultErrorPage.html"; 
    
    // if ( errorPages are given by config file )
    // {
        // std::string path = getPath(); //get user defined path to error pages
        
    // }
    // else
        
    // {

    
    
    // -------- default error page --------
    std::cout << _statusCode << " : " << _reasonPhrase << std::endl;

    std::ifstream       ifs( path );
    std::ostringstream  oss;
    std::string         buffer;    

    if ( !ifs.is_open() )
    {
        std::cerr << "couldnt open file" << std::endl; // not sure if we need error messages here...
        return ;
    }

    oss << ifs.rdbuf();

    if ( ifs.bad() )
    {
        std::cerr << "Reading failed" << std::endl;
        return ;
    }
    buffer = oss.str();
    if ( buffer.empty() )
    {
        std::cerr << "Warning: File is empty" << std::endl;
        return ; 
    }
    
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

    Res.setCodeAndPhrase( std::to_string( _statusCode), _reasonPhrase );
    Res.setHeaders( "Content-Length", std::to_string( buffer.size() ) );
    Res.setHeaders( "Content-Type", getFileType( path ) );
    Res.setBody( buffer );
}


void    PageHandler::setRedirectPage( Response &Res, std::string uri )
{
    std::cout << "ENTER" << std::endl;
    std::string path = "pages/redirectPage.html";

    std::cout << _statusCode << " : " << _reasonPhrase << std::endl;

    std::ifstream       ifs( path );
    std::ostringstream  oss;
    std::string         buffer;    

    if ( !ifs.is_open() )
    {
        std::cerr << "couldnt open file" << std::endl; // not sure if we need error messages here...
        return ;
    }

    oss << ifs.rdbuf();

    if ( ifs.bad() )
    {
        std::cerr << "Reading failed" << std::endl;
        return ;
    }
    buffer = oss.str();
    if ( buffer.empty() )
    {
        std::cerr << "Warning: File is empty" << std::endl;
        return ; 
    }
    
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

    Res.setCodeAndPhrase( std::to_string( _statusCode), _reasonPhrase );
    Res.setHeaders( "Location", uri );
    Res.setHeaders( "Content-Length", std::to_string( buffer.size() ) );
    Res.setHeaders( "Content-Type", getFileType( path ) );
    Res.setBody( buffer );
}


