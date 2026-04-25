#pragma once

#include "HttpException.hpp"

class BadRequest : public HttpException
{
    public:
        BadRequest() : HttpException( 400, "Bad Request" ) {}
        // ~BadRequest();
};