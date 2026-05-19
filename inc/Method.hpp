# pragma once

#include <string>

class Method
{
    private:
        std::string path;
    public:
        Method();
        ~Method();
        void    getMethod();
        void    postMethod();
        void    deleteMethod();
};
