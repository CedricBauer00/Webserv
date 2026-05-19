# pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Exceptions.hpp"

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
