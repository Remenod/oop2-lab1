#include "TaskRunner.h"
#include "ProcessRunner.h"

TaskRunner::TaskRunner(Task t)
    : task(t),
      timer(t.interval, [this]()
            { run(task.binaryFile); })
{
    compile(task.sourceFile, task.binaryFile);
}

void TaskRunner::start()
{
    timer.start();
}

void TaskRunner::stop()
{
    timer.stop();
}