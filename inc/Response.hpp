#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <utility>
#include "Utils.hpp"

class HttpException;

class Response
{
    private:   
        std::string _response;
        std::string _httpVersion;
        std::string _statusCode;
        std::string _reasonPhrase;
        std::unordered_map<std::string, std::string>	_headers;
        std::string _body;
		bool		_internalRedirect = false;

	public:
		Response();
		Response(Response&& other) noexcept;
		Response& operator=(Response&& other) noexcept;
		~Response();

		void				build();
		void				build(std::string statusCode,
			std::string reasonPhrase);
        void				build(std::string&& content,
			std::unordered_map<std::string, std::string>&& headers,
			std::string statusCode,
			std::string reasonPhrase);
		void				build(
			std::unordered_map<std::string, std::string>&& headers,
			std::string statusCode,
			std::string reasonPhrase);
		void				build(HttpException&& e);
		void				setBody(std::string&& content);
		void				setCodeAndPhrase(std::string statusCode,
			std::string reasonPhrase );
		void				setRedirect(unsigned long statusCode);
		void				setHeaders(const std::string& key,
			const std::string& val);
		void				mvBodyToText();
		const std::string&	getText() const;
		bool				wasRedirected() const;
		std::size_t			bodySize() const;
		void				clear();

};
