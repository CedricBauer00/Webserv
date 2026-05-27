#include "../inc/Method.hpp"

Method::Method() {}

Method::~Method() {}

std::string    Method::modifyPath( std::string uri, whichMethod whichMethod )
{
        // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";

    std::string::size_type pos = uri.find( '?' );
    std::string path = uri;
    
    if ( pos != std::string::npos )
    {
        path = uri.substr( 0, pos );
        _query = uri.substr( pos + 1 );
    }

    for ( size_t i = 0; i < path.size(); ++i )
    {
        if ( path[ i ] == '%' && i + 2 < path.size() )
        {
            std::string hex = path.substr( i + 1, 2 );
            char c = static_cast<char>( std::strtol( hex.c_str(), 0, 16 ) ); // 
            path.replace( i, 3, 1, c );
        }
    }

    std::vector<std::string> wholePath;
    std::istringstream iss(path);
    std::string partStr;
    // int i = 0;
    while ( getline( iss, partStr, '/' ) )
    {
        std::cout << "partStr:" << partStr << std::endl;

        if ( partStr.empty() || partStr == "." ) // "." heisst dieses Verzeichnis
            continue ;
        if ( partStr == ".." )
        {
            if ( wholePath.empty() )
                throw BadRequest();
                
            wholePath.pop_back(); // one directory out  
        }
        else
            wholePath.push_back( partStr );
        // std::cout << "wholePath:" << wholePath[ i ] << std::endl;
        // i++;
    }

    std::string newPath;
    newPath = "/";
    for ( size_t i = 0; i < wholePath.size(); ++i )
    {
        newPath += wholePath[ i ];
        if ( i + 1 < wholePath.size() )
            newPath += "/";
    }
    
    // std::cout << "newPath == " << newPath << std::endl; 

    std::string mockLocation = "/images";
    std::string mockRoot = getRootPath();

    if ( whichMethod == METHOD_POST )
    {
        if ( !getUploadEnabled() )
            throw Forbidden();
        if( !( path.empty() ) )
            mockRoot = getUploadPath().empty() ? mockRoot : getUploadPath();
        std::cout << mockRoot << std::endl;
    }

    newPath = mockLocation + newPath;
    // newPath = newPath.substr( mockLocation.size() );
    
    // /DO.PNG
    if ( !( newPath.empty() ) && newPath[ 0 ] == '/' )
        newPath = newPath.substr( 1 );
    if ( mockRoot.back() != '/' )
        mockRoot += '/';
    newPath = mockRoot + newPath;

    // std::cout << "newPath:" << newPath << std::endl;
    
    std::cout << "finished modify path" << std::endl;
    return newPath;
}


void    Method::getMethod( std::string newPath, Response &res, bool _isCgiFile ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::vector<std::string> stack;
    stack.push_back("index1.html");
    stack.push_back("index2.html");
    stack.push_back("index3.html");

    // std::cout << "newPath:" << newPath << std::endl;

    std::error_code ec;
    if ( !( std::filesystem::exists( newPath, ec ) ) )
        throw NotFound();

    if ( std::filesystem::is_regular_file( newPath ) )
    {
        if ( _isCgiFile )
        {
            runCgi(); // put CGI output to response
            return ;
        }
        std::ifstream ifs( newPath ); 
    
        if ( !( ifs.is_open() ) ) // permissions check
            throw NotFound();
        
        std::string content;
        std::ostringstream oss;
        
        oss << ifs.rdbuf();

        content = oss.str();

        res.setBody( content );
        res.setCodeAndPhrase( "200", "OK" );
        res.setHeaders( "Content-Length", std::to_string( content.size() ) );
        res.setHeaders( "Content-Type", getFileType( newPath ) );
        return ;
    }
    else //    if ( std::filesystem::is_directory( newPath ) )
    {
        if ( newPath.back() != '/' )
        {
            std::string newStr = newPath + "/";
            throw MovedPermanently( newStr );
        }
        for ( auto x : stack ) // replace stack with all files in directory - indexes from location 
        {
            std::string joinedPath = newPath + x;
            if ( std::filesystem::exists( joinedPath, ec ) )
            {
                std::ifstream ifs( joinedPath ); 
                std::cout  << joinedPath << std::endl;

                if ( !( ifs.is_open() ) ) // permissions check
                    throw NotFound();
                std::string content;
                std::ostringstream oss;
        
                oss << ifs.rdbuf();

                content = oss.str();

                res.setBody( content );
                res.setCodeAndPhrase( "200", "OK" );
                res.setHeaders( "Content-Length", std::to_string( content.size() ) );
                res.setHeaders( "Content-Type", getFileType( joinedPath ) );
                std::cout << "GET function is done" << std::endl;

                return ;
            }
        }

        if ( autoIndexActive() ) // not implemented yet   
        {
            createAutoIndex( newPath, res ); // not implemented yet
            return ;
        }
        else
            throw NotFound();
    }

    // 1) Method permissions pruefen passiert in execution func - check ob syntax korrekt?
    //  wenn ein body bei GET method - ignoreiren

    // 2) URI - Filesystem-path ( join root + uri, Normalisierung, Traversal-Check )
    // Exists - nein? - 404
    
    // 3) is a directory?
        // ja -> Indexsuche 
        // if found - follow file path
        // if not - check autoindex
        // otherwise 403
    
    // 4) Is a file? 
        // yes - read file
        // get content-type
        // set content-length
        // build response
    // 5) I/O-Fehler - 500

}

bool    getUploadEnabled()
{
    return true;
}

std::string getUploadPath()
{
    return "./servers/server1/uploads";
}

std::string getRootPath()
{
    return "./servers/server1/uploads";
}

// test: printf 'DELETE /images HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
void    Method::deleteMethod( std::string newPath, Response &res ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::cout << "newPath:" << newPath << std::endl;


    std::error_code ec;
    if ( !( std::filesystem::exists( newPath, ec ) ) )
        throw NotFound();

    if ( std::filesystem::is_regular_file( newPath ) )
    {
        std::cout << "Enter delete function" << std::endl;

        std::ifstream ifs( newPath ); 
    
        if ( !( ifs.is_open() ) ) // permissions check
            throw NotFound();
        // delete file
        std::filesystem::remove( newPath );

        res.setCodeAndPhrase( "204", "No Content" );
        res.setHeaders( "Content-Length", "0" );
        return ;
    }
    else // is directory, 403 Forbidden oder wenn delete directory explizit erlaubt ist
    {
        if ( getAllowDeleteDir() == true ) // deleting directory is allowed
        {
            std::filesystem::remove_all( newPath );
            res.setCodeAndPhrase( "204", "No Content" );
            res.setHeaders( "Content-Length", "0" );
            return ;
        }

        throw Forbidden();
    }
}

// test: printf 'POST /images HTTP/1.1\r\nHEAEDER1: A A A A\r\nHEAEDER2: B B B B \r\nHEADER3: C C C C\r\nHoST: example.com\r\n\r\nTHIS IS A BODY\nWith a newline\nand another one\nnewline\nnewline\rA\rD\rC\r\n\r\n' | nc 127.0.0.2 3490
void    Method::postMethod( std::string newPath, Response &res, std::string contentBody, bool _isCgiFile ) // status codes 200, 402, 404
{
    // std::string uri = "/images/cat%20pics/../dog.png?size=large&debug=1";
    std::cout << "newPath:" << newPath << std::endl;

    std::error_code ec;
    if ( !( std::filesystem::exists( newPath, ec ) ) ) // wenn file existiert muessen wir checken, ob wir ueberschreiben duerfen? sonst exception?
        throw NotFound();

        
    if ( std::filesystem::is_regular_file( newPath ) )
    {
        std::cout << "Enter delete function" << std::endl;
        if ( _isCgiFile )
        {
            runCgi(); // put CGI output to response
            return ;
        }

        if ( !getAllowedToOverwrite() )
            throw Forbidden();
        else
        {
            std::ofstream ofs( newPath, std::ios::binary | std::ios::trunc );
            if ( !ofs )
                throw BadRequest();
            //write
            // put content
            ofs << contentBody;
            ofs.close();
            res.setCodeAndPhrase( "200", "OK" );
            res.setHeaders( "Content-Length", "0" );
            return ;
        }        
    }
    else // is directory, 403 Forbidden oder wenn delete directory explizit erlaubt ist
    {
        // create file 
        std::string fileName = "upload";
        fileName += getTimeStamp();
        fileName += ".bin";
        
        std::cout << "FileName:" << fileName << "\n" << "JoinedPath:" << newPath + fileName << "\nBody:\n" << contentBody << std::endl;
        std::ofstream ofs( newPath + fileName );
        
        if ( !ofs )
            throw BadRequest();

        ofs << contentBody;

        ofs.close();
        
        res.setCodeAndPhrase( "201", "Created" );
        res.setHeaders( "Content-Length", "0" );
        return ;
    }
}

std::string Method::getCgiPath()
{
    return "./servers/server1/cgi/python3";
}

std::string getScript()
{
    return "print ('Hello, world!')";
}

void    Method::runCgi()
{
    int inPipe[2];
    int outPipe[2];

    pipe( inPipe );
    pipe( outPipe );

    pid_t pid = fork();

    if ( pid == 0 )
    {
        std::string cgi = getCgiPath();
        std::string script = getScript();
        
        _query += "QUERY_STRING=" + _query;
        char *envp[] = { ( char *)_query.c_str(), NULL };
        char *argv[] = { ( char *)cgi.c_str(), ( char *)script.c_str(), NULL };

        dup2( inPipe[ 0 ], STDIN_FILENO );
        dup2( outPipe[ 1 ], STDOUT_FILENO );
        close( inPipe[ 1 ] );
        close( outPipe[ 0 ] );

        execve( cgi.c_str(), argv, envp ); // returned direkt aus function?

        perror( "execve failed" );
        _exit( 1 );
    }
    else
    {
    }
// else
// {
//     close(inPipe[0]);   // Parent liest nicht von stdin-pipe
//     close(outPipe[1]);  // Parent schreibt nicht in stdout-pipe

//     std::string content;
//     char buffer[1024];
//     ssize_t bytesRead;

//     while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0)
//     {
//         content.append(buffer, bytesRead);
//     }

//     close(outPipe[0]);

//     int status;
//     waitpid(pid, &status, 0);

//     std::cout << "CGI Output:\n" << content << std::endl;
// }
    // }
}

bool    getAllowDeleteDir()
{
    return false;
}

bool    getAllowedToOverwrite()
{
    return false;
}

std::string getTimeStamp()
{
    return std::to_string( std::time( 0 ) );
}


// execve() selbst gibt dir keinen normalen Funktions-Output zurück.
// Wenn execve() erfolgreich ist, ersetzt es den aktuellen Prozess komplett — der Code danach wird nie ausgeführt.

// Den Output des gestarteten Programms bekommst du über die Pipe, die du auf STDOUT_FILENO umgeleitet hast:

// dup2(outPipe[1], STDOUT_FILENO);

// Alles, was das CGI-Script auf stdout schreibt (printf, cout, etc.), kannst du im Parent-Prozess aus outPipe[0] lesen.

// Dein Parent-Teil müsste also ungefähr so aussehen:

// else
// {
//     close(inPipe[0]);   // Parent liest nicht von stdin-pipe
//     close(outPipe[1]);  // Parent schreibt nicht in stdout-pipe

//     std::string content;
//     char buffer[1024];
//     ssize_t bytesRead;

//     while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0)
//     {
//         content.append(buffer, bytesRead);
//     }

//     close(outPipe[0]);

//     int status;
//     waitpid(pid, &status, 0);

//     std::cout << "CGI Output:\n" << content << std::endl;
// }

// Wichtig:

// outPipe[1] ist die Schreibseite des Childs
// outPipe[0] ist die Leseseite des Parents
// Das CGI-Programm muss auf stdout schreiben

// Beispiel:

// std::cout << "Hello CGI";

// oder in C:

// printf("Hello CGI");

// landet dann in content.

// Zusätzlich solltest du im Child nach dup2() beide ursprünglichen Pipe-FDs schließen:

// close(inPipe[0]);
// close(inPipe[1]);
// close(outPipe[0]);
// close(outPipe[1]);

// denn nach dup2() werden sie nicht mehr direkt gebraucht.



// ////////

// Ja, genau so funktioniert fork().

// Nach dem fork() existieren zwei Prozesse, die beide ab der nächsten Zeile weiterlaufen:

// der Child-Prozess
// der Parent-Prozess

// Beide führen denselben Code weiter aus, aber mit unterschiedlichem Rückgabewert von fork():

// pid_t pid = fork();
// pid == 0 → du bist im Child
// pid > 0 → du bist im Parent
// pid < 0 → Fehler

// Deshalb:

// if (pid == 0)
// {
//     // Child-Prozess
// }
// else
// {
//     // Parent-Prozess
// }

// laufen gleichzeitig in zwei verschiedenen Prozessen.

// Der Child macht bei dir:

// dup2() → stdout auf Pipe umleiten
// execve() → startet CGI-Programm

// Das CGI schreibt dann nach stdout.

// Da stdout jetzt auf:

// outPipe[1]

// zeigt, landet alles in der Pipe.

// Der Parent liest parallel aus:

// outPipe[0]

// und bekommt dadurch die Ausgabe des Childs.

// Das ist exakt der klassische UNIX-Weg für:

// Parent ↔ Child Kommunikation
// CGI
// Shell Pipes
// popen()
// Terminal-Pipelines (ls | grep txt)

// Wichtig zu verstehen:

// Die Pipe ist ein Kernel-Puffer zwischen beiden Prozessen:

// Child stdout ---> outPipe[1]  ===== KERNEL PIPE ===== outPipe[0] ---> Parent read()

// Noch ein wichtiger Punkt:

// execve(...)

// erstellt keinen neuen Prozess.

// fork() erstellt den neuen Prozess.

// execve() ersetzt nur den aktuellen Child-Prozess durch ein anderes Programm.

// Also:

// Parent
//    |
// fork()
//    |
//    +---- Child
//              |
//              +---- execve() -> jetzt läuft CGI statt deines ursprünglichen Codes

// Darum kehrt execve() bei Erfolg nie zurück.


// //////////////////////


// Fast — ein wichtiger Punkt fehlt:

// execve() returned nicht bei Erfolg.

// Das bedeutet:

// execve(cgi.c_str(), argv, envp);

// macht intern ungefähr:

// "Ersetze diesen Child-Prozess komplett durch das CGI-Programm"

// Wenn das klappt:

// dein bisheriger Child-Code verschwindet
// stattdessen läuft jetzt das CGI-Programm
// der Code nach execve() wird NIE ausgeführt

// Darum:

// perror("execve failed");

// läuft nur wenn execve() fehlgeschlagen ist.

// Der wichtige Zusammenhang ist:

// Vor execve() hast du bereits:

// dup2(outPipe[1], STDOUT_FILENO);

// gemacht.

// Dadurch zeigt stdout jetzt nicht mehr auf das Terminal, sondern auf die Pipe.

// Normalerweise:

// stdout --> Terminal

// Nach dup2():

// stdout --> outPipe[1]

// Und genau dieses umgeleitete stdout erbt das neue Programm nach execve().

// Das CGI merkt davon gar nichts.

// Es macht einfach:

// printf("Hello");

// oder:

// std::cout << "Hello";

// und denkt:

// "Ich schreibe auf stdout"

// Aber tatsächlich landet es in:

// outPipe[1]

// Der Parent liest dann aus:

// read(outPipe[0], ...)

// und bekommt "Hello".

// Also die Reihenfolge ist:
// fork()
//    |
//    +-- Child
//          |
//          +-- dup2(pipe -> stdout)
//          |
//          +-- execve(CGI)
//                  |
//                  +-- CGI schreibt auf stdout
//                          |
//                          +-- landet in outPipe[1]

// Parent
//    |
//    +-- read(outPipe[0])
//            |
//            +-- bekommt CGI-Output