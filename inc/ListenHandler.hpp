#pragma once

#include <string>
#include <map>
#include "EventHandler.hpp"

class ListenHandler: public EventHandler
{
    public:
        ListenHandler(int fd): EventHandler(fd) {};
        virtual ~ListenHandler() {};
};