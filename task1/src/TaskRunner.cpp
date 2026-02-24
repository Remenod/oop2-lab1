#include "TaskRunner.h"
#include "ProcessRunner.h"

TaskRunner::TaskRunner(const Task &t)
    : task(t),
      timer(t.interval, [this]()
            { run(task.dir + "/task"); })
{
    compile(task.dir + "/task.cpp", task.dir + "/task");
}

void TaskRunner::start()
{
    if (!timer.isRunning())
        timer.start();
}

void TaskRunner::stop()
{
    if (timer.isRunning())
        timer.stop();
}