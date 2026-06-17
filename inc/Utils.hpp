#pragma once 

#include <map>
#include <string>
#include <vector>
#include <filesystem>

class Response;

std::string setStatus(int code);
std::string getFileType(const std::string& path);
std::string getExtension(const std::string& contentType);