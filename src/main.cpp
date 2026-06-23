#include <iostream>

#include "../inc/Configparsing/ConfigParser.hpp"
#include "../inc/WebServ.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
        return 0;

    try {
		WebServ webserv(argv[1]);
		webserv.run();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}