# pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Exceptions.hpp"
#include <filesystem>
#include <fstream>
#include "Response.hpp"

class Method
{
    private:
        std::string _path;
    public:
        Method();
        ~Method();
        void    getMethod( Response& res);
        void    postMethod( Response& res);
        void    deleteMethod( Response& res);
};
