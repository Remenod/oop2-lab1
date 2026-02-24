#include "Daemon.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>

static const char *FIFO_PATH = "runtime/taskd.fifo";

static const char *TASKS_DIR = "tasks/";

static Task loadTask(const std::string &name)
{
    Task t;
    t.name = name;
    t.dir = std::string(TASKS_DIR) + name;

    std::ifstream cfg(t.dir + "/config.cfg");
    std::string line;
    while (std::getline(cfg, line))
    {
        if (line.starts_with("interval="))
            t.interval = std::stoi(line.substr(9));
    }

    return t;
}

void Daemon::handleCommand(const std::string &line)
{
    std::istringstream iss(line);
    std::string cmd, name;
    iss >> cmd >> name;

    if (cmd == "START")
    {
        if (!tasks.count(name))
        {
            Task t = loadTask(name);
            tasks[name] = std::make_unique<TaskRunner>(t);
        }
        tasks[name]->start();
        std::cout << "[daemon] task " << name << " started\n";
    }
    else if (cmd == "STOP")
    {
        if (tasks.count(name))
        {
            tasks[name]->stop();
            tasks.erase(name);
            std::cout << "[daemon] task " << name << " stoped\n";
        }
    }
    else if (cmd == "SET_INTERVAL")
    {
        int newInterval;
        iss >> newInterval;

        if (tasks.count(name))
        {
            tasks[name]->stop();
            tasks[name]->setInterval(newInterval);
            tasks[name]->start();
            std::cout << "[daemon] task " << name << " interval set to " << newInterval << "\n";
        }
        else
        {
            std::cout << "[daemon] task " << name << " not running, config updated\n";
        }
    }
    else if (cmd == "EXIT")
    {
        for (auto &[name, taskPtr] : tasks)
            taskPtr->stop();
        tasks.clear();
        if (fifoFd >= 0)
            close(fifoFd);
        std::cout << "[daemon] stoped\n";
        exit(0);
    }
}

void Daemon::run()
{
    mkdir("runtime", 0755);
    mkfifo(FIFO_PATH, 0666);

    fifoFd = open(FIFO_PATH, O_RDONLY);
    if (fifoFd < 0)
    {
        perror("open fifo");
        return;
    }

    std::cout << "[daemon] started (foreground)\n";

    char buf[256];
    while (true)
    {
        ssize_t n = read(fifoFd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = 0;
            handleCommand(buf);
        }
    }
}