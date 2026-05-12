#include "../inc/Execution.hpp"

Execution::Execution() {}

Execution::~Execution() {}

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    Execution::execution( std::string request, Response &Res )
{
    try
    {
        HttpParser parser( request );

        // 1) parse request
        parser.setHeaders();

        Method m = parser.getMethod();
        
        
        // 2)   SERVER_REWRITE
        //      server{} rw

        serverRewrite( parser.getUri() );

        // 3)   FIND_CONFIG
        //      location{}
        //      korrekten Server & Location finden anhand von HOST/PORT/URI

        // 4)   REWRITE
        //      location{} rw

        // 5)   POST_REWRITE -> rewrite last / URI changed

        // 6)   PRE_ACCESS
        //      limit_req

        // 7)   ACCESS
        //      auth/allow

        // 8)   POST_ACCESS -> access denied -> [SEND 401/403]

        // 9)   PRE_CONTENT
        //      try_files

        // 10)  CONTENT
        //      proxy/static

        // 11)  LOGGING (acces-/error-log)

        // if ( m == METHOD_GET )
        //     getLogic();
        // else if ( m == METHOD_DELETE )
        //     deleteLogic();
        // else if ( m == METHOD_POST )
        if ( m == METHOD_POST )
        {
            parser.setBody(); // for POST requests - last step of execution
            // postLogic();
        }
        std::cout << ORANGE << parser.getBody() << RESET << std::endl;

        /// Response Buidling 
        Res.build();
    }
    catch ( const HttpException& e )
    {
        PageHandler pageHandler( e.getStatusCode(), e.getReasonPhrase() );
        if ( e.getStatusCode() == 301 || e.getStatusCode() == 302 )
        {
            pageHandler.setRedirectPage( Res, e.getLocation() );
        }
        else
            pageHandler.setErrorPage( Res );
        Res.build();
    }
}

struct RewriteRule {
    std::string pattern;      // z.B. "^/old/(.*)$"
    std::string replacement;  // z.B. "/new/$1"
    bool redirect;            // true = 301/302
    int code;                 // 301 oder 302
};

void    Execution::serverRewrite( std::string uri ) //rewriting URI based on rules in config??
{
    _uri = uri; // 1)   copy URI from request 
    
    // 2)   choosing server based on Host/Port
    //      Server rewrite rules
    RewriteRule rule;
    rule.pattern = "/old/";
    rule.replacement = "/new/";
    rule.redirect = true;
    rule.code = 301; //depending on server block
    
    // 3)   Reading rules
    //      checking if rules can be applied
    if ( _uri.compare( 0, rule.pattern.size(), rule.pattern ) == 0 )
    {
        // 4)   modifying new URI 
        //      if rule is redirect - build response (301/302) - exit
        //      if only internally - modify URI - continue
        std::string newUri = rule.replacement + _uri.substr( rule.pattern.size() );
        std::cout << "newUri=" << newUri << std::endl;
        if ( rule.redirect )
        {
            if ( rule.code == 301 )
                throw MovedPermanently( newUri );
            else if ( rule.code == 302 )
                throw Found( newUri );
            // "request should not be handled here!"
            // Client has to request different URL
        }
        else
            _uri = newUri; // 5)   continue with new URI
    }
    // 6)   location matching with new URI 
}

// printf 'GET /Something HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490

// server {
//     listen 3490;
//     server_name example.com;

//         # Internes Rewrite (kein Redirect)
//     rewrite ^/old/(.*)$ /new/$1 last; - internal rewrite, then choose location

//         # Internes Rewrite (z.B. /docs -> /docs/)
//     rewrite ^/docs$ /docs/ last;

//         # 301 Permanent Redirect
//     rewrite ^/legacy$ /new permanent; - 301

//        # 302 Temporary Redirect
//     rewrite ^/beta$ /new redirect; - 302

//     location /new/ {
//         root /var/www/site;
//     }

//     location / {
//         root /var/www/default;
//     }
// }