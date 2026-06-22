#pragma once

#include <string>
#include <unordered_map>

const std::unordered_map<unsigned long, std::string> statusCodeToReasonPhrase = {
	{100, "Continue"},
	{200, "OK"},
	{201, "Created"},
	{204, "No Content"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{413, "Payload Too Large"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{505, "Http Version Not Allowed"}
};