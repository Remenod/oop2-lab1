#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include "TaskRunner.h"

class Daemon
{
    std::unordered_map<std::string, std::unique_ptr<TaskRunner>> tasks;
    int fifoFd = -1;

public:
    void run();
    void handleCommand(const std::string &line);
};