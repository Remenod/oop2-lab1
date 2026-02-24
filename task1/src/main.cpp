#include "Daemon.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

static const char *FIFO_PATH = "runtime/taskd.fifo";

static const char *TASKS_DIR = "tasks/";

void createTaskDir(const std::string &name, int interval)
{
    std::string path = std::string(TASKS_DIR) + name;
    mkdir(TASKS_DIR, 0755);
    mkdir(path.c_str(), 0755);

    std::ofstream cfg(path + "/config.cfg");
    cfg << "interval=" << interval << "\n";
    cfg.close();

    std::ofstream cpp(path + "/task.cpp");
    cpp << "#include <iostream>\nint main() { std::cout << \"" << name << "\\n\"; return 0; }";
    cpp.close();

    std::cout << "[CLI] task '" << name << "' created with interval=" << interval << "\n";
}

void editTask(const std::string &name)
{
    std::string path = std::string(TASKS_DIR) + name + "/task.cpp";

    if (access(path.c_str(), F_OK) != 0)
    {
        std::cerr << "[CLI] task '" << name << "' does not exist\n";
        return;
    }

    int ret = std::system(("nano " + path).c_str());
    if (ret != 0)
    {
        std::cerr << "[CLI] unable to open nano\n";
    }
}

void setIntervalTask(const std::string &name, int interval)
{
    std::string cfgPath = std::string(TASKS_DIR) + name + "/config.cfg";

    if (access(cfgPath.c_str(), F_OK) != 0)
    {
        std::cerr << "[CLI] task '" << name << "' does not exist\n";
        return;
    }

    std::ofstream cfg(cfgPath);
    cfg << "interval=" << interval << "\n";

    std::cout << "[CLI] task '" << name << "' interval set to " << interval << "\n";
}

void listTasks()
{
    namespace fs = std::filesystem;

    if (!fs::exists(TASKS_DIR))
    {
        std::cout << "[CLI] no tasks directory found\n";
        return;
    }

    for (auto &entry : fs::directory_iterator(TASKS_DIR))
    {
        if (entry.is_directory())
        {
            std::string name = entry.path().filename().string();
            std::string cfgPath = entry.path().string() + "/config.cfg";
            int interval = 0;

            std::ifstream cfg(cfgPath);
            std::string line;
            while (std::getline(cfg, line))
            {
                if (line.starts_with("interval="))
                    interval = std::stoi(line.substr(9));
            }

            std::cout << name << " (interval=" << interval << ")\n";
        }
    }
}

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
    else if (cmd == "restart-task" && argc == 3)
    {
        send("STOP " + std::string(argv[2]) + "\n");
        send("START " + std::string(argv[2]) + "\n");
    }
    else if (cmd == "add-task" && argc == 4)
    {
        std::string name = argv[2];
        int interval = std::stoi(argv[3]);
        createTaskDir(name, interval);
    }
    else if (cmd == "edit-task" && argc == 3)
    {
        std::string name = argv[2];
        editTask(name);
    }
    else if (cmd == "set-interval" && argc == 4)
    {
        std::string name = argv[2];
        int interval = std::stoi(argv[3]);
        setIntervalTask(name, interval);
        send("SET_INTERVAL " + name + " " + std::to_string(interval) + "\n");
    }
    else if (cmd == "list-tasks")
    {
        listTasks();
    }
    else if (cmd == "stop-daemon")
    {
        send("EXIT\n");
    }
}