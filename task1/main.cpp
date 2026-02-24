#include "TaskRunner.h"
#include <vector>
#include <memory>
#include <iostream>

int main()
{
    std::vector<std::unique_ptr<TaskRunner>> tasks;

    while (true)
    {
        std::cout << "interval source.cpp > ";
        Task t;
        std::cin >> t.interval >> t.sourceFile;
        t.binaryFile = t.sourceFile + ".out";

        auto runner = std::make_unique<TaskRunner>(t);
        runner->start();
        tasks.push_back(std::move(runner));
    }
}