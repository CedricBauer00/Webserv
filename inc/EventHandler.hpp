#pragma once

#include <string>
#include <map>
#include <iostream>

class EventHandler
{
    protected:
        int _fd;
        // std::vector<Conf> server_blocks
        // server_block[0].server_name[0] 
    public:
        EventHandler(int fd): _fd(fd) {};
        virtual ~EventHandler() {};
        virtual int getFd() {return _fd;};
};
