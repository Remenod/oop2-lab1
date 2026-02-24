#include "ProcessRunner.h"
#include <cstdlib>

int compile(const std::string &src, const std::string &out)
{
    std::string cmd = "g++ " + src + " -O2 -o " + out;
    return std::system(cmd.c_str());
}

void run(const std::string &binary)
{
    std::system(("./" + binary).c_str());
}