#include <iostream>
#include <exception>
#include <functional>
#include <vector>
#include <cmath>
#include <sstream>
#include <string>

int main()
{
    std::vector<std::function<double(double)>> jobs =
        {
            [](double x)
            { return std::sqrt(std::abs(x)); },
            [](double x)
            { return std::pow(x, 3); },
            [](double x)
            { return x + 3.5; },
        };

    std::cout << "0 <num> - sqrt(num)\n"
                 "1 <num> - num^3\n"
                 "2 <num> - num + 3.5\n";

    try
    {
        while (true)
        {
            int jobIndex;
            double x;

            std::string line;
            std::getline(std::cin, line);

            std::istringstream iss(line);

            iss.exceptions(std::ios::failbit | std::ios::badbit);

            iss >> jobIndex >> x;

            if (!iss.eof())
                throw std::ios_base::failure("too much data");

            std::cout << jobs.at(jobIndex)(x) << std::endl;
        }
    }
    catch (const std::ios_base::failure &)
    {
        std::cout << "Invalid input. Exiting.\n";
    }
    catch (const std::out_of_range &)
    {
        std::cout << "Job index out of range. Exiting.\n";
    }
}