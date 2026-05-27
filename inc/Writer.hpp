#pragma once

#include "Reader.hpp"
#include "Response.hpp"

class Writer: public AEventHandler {
    private:
        Response	_res;
        size_t		_sentBytes{0};

    public:
        Writer() = delete;
        Writer(const int fd, const Reader& reader);
        virtual ~Writer();

        void	process(uint32_t events) override;
};