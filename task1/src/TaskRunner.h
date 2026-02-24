#pragma once
#include "Task.h"
#include "Timer.h"

class TaskRunner
{
    Task task;
    Timer timer;

public:
    TaskRunner(const Task &t);
    void start();
    void stop();
    void setInterval(int seconds);
};