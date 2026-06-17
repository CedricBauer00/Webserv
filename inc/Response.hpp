#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <utility>
#include "Utils.hpp"

class Response
{
    private:   
        std::string _response;
        std::string _httpVersion;
        std::string _statusCode;
        std::string _reasonPhrase;
        std::unordered_map<std::string, std::string>	_headers;
        std::string _body;

	public:
		Response();
		Response(Response&& other) noexcept;
			Response& operator=(Response&& other) noexcept;
		~Response();

		void				build();
		void				setBody(std::string&& content);
		void				setCodeAndPhrase(const std::string& statusCode,
			const std::string& reasonPhrase );
		void				setHeaders(const std::string& key,
			const std::string& val);
		const std::string&	getResponse() const;
		void				clear();

};

// std::string setStatus(int code);
