#pragma once

#include <string>

class Response
{
    private:   
        std::string _response;
    public:
        Response();
        ~Response();

        void        setResponse( std::string Response );
        void        setErrorPage( std::string ErrorPage );
        std::string getResponse() const;
};