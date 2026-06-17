#pragma once 

#include <map>
#include <string>
#include <vector>
#include <filesystem>

class Response;

std::string setStatus(int code);
std::string getFileType( std::string path );
// bool        autoIndexActive();
void        createAutoIndex( std::string mockUri, Response &res );
std::string getExtension( std::string type );