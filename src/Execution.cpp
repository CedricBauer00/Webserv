#include "../inc/Execution.hpp"
#include "../inc/Method.hpp"
#include "../inc/Configparsing/WebservCoreModule.hpp"

Execution::Execution() {}

Execution::~Execution() {}

const IWebservModule::Srv*	Execution::selectServer(
    const std::string& hostname,
	const std::vector<const IWebservModule::Srv*>& servers) {
	const IWebservModule::Srv* defaultSrv = nullptr;
	for (const IWebservModule::Srv* srv : servers) {
		if (srv->srvConfs.empty())
			continue;
		const WebservCoreParser::SrvCoreConf* srvConf =\
		dynamic_cast<WebservCoreParser::SrvCoreConf*>(srv->srvConfs[0].get());
		const std::unordered_set<std::string>& names = srvConf->serverNames;
		if (names.find(hostname) != names.end())
			return srv;
		if (srvConf->flags & DEFAULT_SERVER)
			defaultSrv = srv;
	}
	return defaultSrv != nullptr ? defaultSrv : servers[0];
}

const IWebservModule::LocNode*	Execution::selectLocation(const std::string& uri,
	const IWebservModule::LocNode& root) {
	const IWebservModule::LocNode* bestMatch = nullptr;
	std::deque<const IWebservModule::LocNode*> queue;
	queue.push_back(&root);
	while (!queue.empty()) {
		const IWebservModule::LocNode* node = queue.front();
		queue.pop_front();
		if (node->matchType == 0 && node->name == uri)
			return node;
		else if (node->matchType == 1 && uri.compare(0, node->name.size(), node->name) == 0) {
			if (bestMatch == nullptr || node->name.size() > bestMatch->name.size())
				bestMatch = node;
		}
		for (const auto& loc : node->locations)
			queue.push_back(loc.get());
	}
	return bestMatch;
}

// This function is ment to contain all relevant steps for the execution - ich bin mir noch nicht sicher ob das hier Sinn macht...
// Hier kannst du gerne deine execution Logic skizzieren
void    Execution::execution(
    const std::string& request,
    Response &res,
    const std::vector<const IWebservModule::Srv*>& servers)
{
    const IWebservModule::Srv* server = nullptr;
    try
    {
        HttpParser parser( request );

        // 1) parse request
        parser.setHeaders();

        Method  m;
		
        std::string normalizedUri = m.normalizePath( parser.getUri() );
		std::cout << "Normalized Uri: " << normalizedUri << std::endl;

        server = selectServer(parser.getHostName(), servers); // select server based on Host name
		for (const auto& item :
			dynamic_cast<WebservCoreParser::SrvCoreConf*>(
				server->srvConfs[0].get())->serverNames) {
			std::cout << item << ", ";
		}
		std::cout << '\n';
		const IWebservModule::LocNode* location = selectLocation(normalizedUri, *server->location); // select location based on URI
        std::cout << "Selected location: '" << location->name << "' with match type " << location->matchType << "\n";

		whichMethod whichMethod = parser.getMethod();
        
        // 2)   SERVER_REWRITE
        //      server{} rw
        // serverRewrite( parser.getUri(), servers, parser.getHostName(), parser.getHostPort() );


        // 3)   FIND_CONFIG
        //      location{}
        //      korrekten Server & Location finden anhand von HOST/PORT/URI
        //      location matching with new URI 

        
        // 4)   REWRITE
        //      location{} rw
        //      POST_REWRITE -> rewrite last / URI changed
        

        // 7)   ACCESS
        //      auth/allow
        //      -> access denied -> [SEND 401/403]

        // 9)   PRE_CONTENT
        //      try_files

        // 10)  CONTENT
        //      proxy/static

        //validate path
        

        std::string joinedPath = m.joinRootAndPath( normalizedUri, whichMethod );
        std::cout << "joinedPath: " << joinedPath << std::endl;

        if ( whichMethod == METHOD_GET )
            m.getMethod( joinedPath, res );
        else if ( whichMethod == METHOD_DELETE )
            m.deleteMethod( joinedPath, res );
        if ( whichMethod == METHOD_POST )
        {
            parser.setBody(); // for POST requests - last step of execution
            m.postMethod( joinedPath, res, parser.getBody() );
            std::cout << ORANGE << parser.getBody() << RESET << std::endl;
        }
        /// Response Buidling 
        res.build();
        std::cout << ORANGE << res.getResponse() << RESET << std::endl;
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

// struct ServerConfig
// {
//     int         listenPort;
//     std::string serverName;
//     std::vector<RewriteRule> rewriteRules;
// };

// void    initRules( std::vector<RewriteRule>& rewriteRules )
// {
//     rewriteRules.push_back({"/old/", "/new/", false, 0 });
//     //  printf 'GET /old/location/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nhOST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
//     rewriteRules.push_back({"/legacy", "/new", true, 301 });
//     //  printf 'GET /legacy/location/ HTTP/1.1\r\nHOST: EXAMPLE.COM\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
//     rewriteRules.push_back({"/beta", "/new", true, 302 });
//     //  printf 'GET /beta/location/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHoST: example.com\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
//     //  printf 'GET /beta/location/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490

// }

// void    Execution::serverRewrite(
//     std::string uri,
//     const std::vector<const IWebservModule::Srv*>&  servers,
//     std::string hostName,
//     std::string hostPort ) // wird vorher gecheckt, welcher Serverblock die Request verarbeitet?
// {
//     _uri = uri;
//     ServerConfig srv;
//     initRules( srv.rewriteRules );

//     (void)servers;
//     (void)hostName;
//     (void)hostPort;
    
//     // bool    portFound = false;
//     // bool    nameFound = false;
        
//     // for ( auto x : servers )
//     // {
//     //     // vorher socket port checken. Also auf welchem Port die Verbindung reinkam
//     //     if ( hostPort && hostPort == std::to_string( x.getPort() ) ) //Schritt 1: Port bestimmen. Wenn im Host‑Header ein Port steht (Host: example.com:8080) → nutze 8080. Schritt 2: Server‑Blöcke nach Port filtern. Du schaust nur die Server an, die auf diesem Port lauschen.
//     //     {
//     //         portForund = true;
//     //         for (  )
//     //         {
//     //             std::cout << "\n" << x.getServerName() << "\n" << x.getDomain() << "\n" << x.getPort() << std::endl;
//     //             if ( x.getServerName() == hostName ) // Schritt 3: Host‑Header gegen server_name. 
//     //             {
//     //                 std::cout << "Server_name matched:\nserver_name: " << x.getServerName() << "\nHostName: " << hostName << std::endl; 
//     //                 matchFound = true;
        
//     //             }
//     //         }
//     //         if ( matchFound == false ) //Schritt 4: Kein Match → Default‑Server
//     //         {
//     //             // use default server
//     //         }
            
//     //     }
//     // }
//     // if ( portFound == false ) // Sonst → nimm den Socket‑Port, also den Port, auf dem die Verbindung angekommen ist. Verbindung kommt auf Port 3490 an. Host‑Header ist example.com (ohne Port)→ Port = 3490
//     // {
//     //     // use socket port
//     // }

//     // 2)   choosing server based on Host/Port !!! HIER WUERDE ICH CHECKEN 
//     //      Server rewrite rules
    

    // std::cout << "\nURI before = " << _uri << std::endl;
    // 3)   Reading rules
    //      checking if rules can be applied
//     for ( size_t i = 0; i < srv.rewriteRules.size(); ++i )
//     {
//         const RewriteRule& rule = srv.rewriteRules[ i ];
//         if ( _uri.compare( 0, rule.pattern.size(), rule.pattern ) == 0 )
//         {
//             std::string newUri = rule.replacement + _uri.substr( rule.pattern.size() ); // 4) modifying new URI 
//             std::cout << "newUri=" << newUri << std::endl;
//             if ( rule.redirect ) // if rule is redirect - build response (301/302) - exit
//             {
//                 if ( rule.code == 301 )
//                     throw MovedPermanently( newUri );
//                 else if ( rule.code == 302 )
//                     throw Found( newUri );
//                 // "request should not be handled here!"
//                 // Client has to request different URL
//             }
//             else // if only internally - modify URI - continue
//                 _uri = newUri; // 5) continue with new URI
//             break;    
//         }
//     }
//     // std::cout << "URI after = " << _uri << std::endl;
// }

// server {
//     listen 3490;
///    server_name home.com;
//      ...
//      ...
//
// }

// server {
//     listen 3490 default;
//     listen 127.0.0.1:443;
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
//         rewrite ^/beta$ /new redirect; - 302
//         root /var/www/site;
//     }

//     location / {
//         root /var/www/default;
        //     location /api {
        //         root....
        //         location /api/42 {
        //             root ...
        //             locat
        //         }
        //     }
        // location /api {
        //     root...;
        // }
//     }
// }

// server {
//      listen 3490;
//         server_name webserv.com;
// }

// location ^/n

// location *[.png]

// localhost/api/


    //  printf 'GET /servers/server1/cgi/test.py/ HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nhOST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
