#pragma once

#include <string>
#include <map>
#include <iostream>

class EventHandler
{
    protected:
        int _fd;
    public:
        EventHandler(int fd): _fd(fd) {};
        virtual ~EventHandler() {};
        virtual int getFd() {return _fd;};
};
