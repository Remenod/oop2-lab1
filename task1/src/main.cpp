#include "Daemon.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

static const char *FIFO_PATH = "runtime/taskd.fifo";

void send(const std::string &s)
{
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd < 0)
    {
        std::cerr << "daemon not running\n";
        return;
    }
    write(fd, s.c_str(), s.size());
    close(fd);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "usage:\n"
                     "  app daemon\n"
                     "  app start-task <name>\n"
                     "  app stop-task <name>\n"
                     "  app stop-daemon\n";
        return 0;
    }

    std::string cmd = argv[1];

    if (cmd == "daemon")
    {
        Daemon d;
        d.run();
    }
    else if (cmd == "start-task" && argc == 3)
    {
        send("START " + std::string(argv[2]) + "\n");
    }
    else if (cmd == "stop-task" && argc == 3)
    {
        send("STOP " + std::string(argv[2]) + "\n");
    }
    else if (cmd == "stop-daemon")
    {
        send("EXIT\n");
    }
}