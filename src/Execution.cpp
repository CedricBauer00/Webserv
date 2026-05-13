#include "../inc/Execution.hpp"

Execution::Execution() {}

Execution::~Execution() {}

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    Execution::execution( std::string request, Response &res, std::vector<Server> servers )
{
    try
    {
        HttpParser parser( request );

        // 1) parse request
        parser.setHeaders();

        Method m = parser.getMethod();
        
        
        // 2)   SERVER_REWRITE
        //      server{} rw

        serverRewrite( parser.getUri(), servers );

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
        res.build();
    }
    catch ( const HttpException& e )
    {
        PageHandler pageHandler( e.getStatusCode(), e.getReasonPhrase() );
        if ( e.getStatusCode() == 301 || e.getStatusCode() == 302 )
        {
            pageHandler.setRedirectPage( res, e.getLocation() );
        }
        else
            pageHandler.setErrorPage( res );
        res.build();
    }
}

struct ServerConfig
{
    int         listenPort;
    std::string serverName;
    std::vector<RewriteRule> rewriteRules;
};

void    initRules( std::vector<RewriteRule>& rewriteRules )
{
    rewriteRules.push_back({"/old/", "/new/", false, 0 });
    //  printf 'GET /old/location/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
    rewriteRules.push_back({"/legacy", "/new", true, 301 });
    //  printf 'GET /legacy/location/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
    rewriteRules.push_back({"/beta", "/new", true, 302 });
    //  printf 'GET /beta/location/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
}

void    Execution::serverRewrite( std::string uri, std::vector<Server> servers ) // wird vorher gecheckt, welcher Serverblock die Request verarbeitet?
{
    _uri = uri;
    ServerConfig srv;
    initRules( srv.rewriteRules );


    // 2)   choosing server based on Host/Port 
    //      Server rewrite rules
    (void)servers;

    std::cout << "URI before = " << _uri << std::endl;
    // 3)   Reading rules
    //      checking if rules can be applied
    for ( size_t i = 0; i < srv.rewriteRules.size(); ++i )
    {
        const RewriteRule& rule = srv.rewriteRules[ i ];
        if ( _uri.compare( 0, rule.pattern.size(), rule.pattern ) == 0 )
        {
            std::string newUri = rule.replacement + _uri.substr( rule.pattern.size() ); // 4) modifying new URI 
            std::cout << "newUri=" << newUri << std::endl;
            if ( rule.redirect ) // if rule is redirect - build response (301/302) - exit
            {
                if ( rule.code == 301 )
                    throw MovedPermanently( newUri );
                else if ( rule.code == 302 )
                    throw Found( newUri );
                // "request should not be handled here!"
                // Client has to request different URL
            }
            else // if only internally - modify URI - continue
                _uri = newUri; // 5) continue with new URI
            break;    
        }
    }
    std::cout << "URI after = " << _uri << std::endl;
    // 6)   location matching with new URI 
}

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