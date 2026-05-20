# pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Exceptions.hpp"
#include <filesystem>
#include <fstream>

class Method
{
    private:
        std::string _path;
    public:
        Method();
        ~Method();
        void    getMethod();
        void    postMethod();
        void    deleteMethod();
};
