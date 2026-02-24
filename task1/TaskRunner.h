#pragma once
#include "Task.h"
#include "Timer.h"

class TaskRunner
{
    Task task;
    Timer timer;

public:
    TaskRunner(Task t);
    void start();
    void stop();
};