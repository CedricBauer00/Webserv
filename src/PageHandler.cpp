#include "../inc/PageHandler.hpp"

PageHandler::PageHandler( int statusCode, std::string reasonPhrase ) : _statusCode( statusCode ), _reasonPhrase( reasonPhrase ) {}

PageHandler::~PageHandler() {}

void    PageHandler::setErrorPage( Response &res )
{
    std::string path;

    if ( errorPages.count( _statusCode ) )
        path = errorPages[ _statusCode ]; 
    else
        path = "pages/defaultErrorPage.html"; 
    
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

    res.setCodeAndPhrase( std::to_string( _statusCode), _reasonPhrase );
    res.setHeaders( "Content-Length", std::to_string( buffer.size() ) );
    res.setHeaders( "Content-Type", getFileType( path ) );
    res.setBody( buffer );
}


void    PageHandler::setRedirectPage( Response &res, std::string uri )
{
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

    res.setCodeAndPhrase( std::to_string( _statusCode), _reasonPhrase );
    res.setHeaders( "Location", uri );
    res.setHeaders( "Content-Length", std::to_string( buffer.size() ) );
    res.setHeaders( "Content-Type", getFileType( path ) );
    res.setBody( buffer );
}

void    PageHandler::initErrorPages( const std::map<int, std::string>& configErroPages )
{
    errorPages.clear();
    for ( std::map<int, std::string>::const_iterator it = configErroPages.begin(); it != configErroPages.end(); ++it )
    {
        errorPages[ it->first ] = it->second;
    }
}

// call fuer error pages auf server ebene
// std::map<int, std::string> configErrorPages;
// std::ifstream config( "webserv.config" );
// std::string line;
// while ( std::getline( config, line ) )
// {
//     if ( line.find( "error_page" ) == 0 )
//     {
//         std::istringstream iss( line );
//         std:;string directive;
//         int code;
//         std::string path;
//         iss >> directive >> code >> path;
//         configErrorPages[ code ] = path;
//     }
// }
// PageHandler.initErrorPages( configErrorPages );