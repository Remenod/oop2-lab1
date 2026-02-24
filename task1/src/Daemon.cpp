#include "Daemon.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>

static const char *FIFO_PATH = "runtime/taskd.fifo";

static Task loadTask(const std::string &name)
{
    Task t;
    t.name = name;
    t.dir = "tasks/" + name;

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
    }
    else if (cmd == "STOP")
    {
        if (tasks.count(name))
            tasks[name]->stop();
    }
}

void Daemon::run()
{
    mkdir("runtime", 0755);
    mkfifo(FIFO_PATH, 0666);

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd < 0)
    {
        perror("open fifo");
        return;
    }

    std::cout << "[daemon] started (foreground)\n";

    char buf[256];
    while (true)
    {
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = 0;
            handleCommand(buf);
        }
    }
}