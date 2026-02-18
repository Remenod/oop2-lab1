#include <iostream>
#include <exception>
#include <functional>
#include <vector>
#include <cmath>

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

    std::cin.exceptions(std::ios::failbit | std::ios::badbit);
    try
    {
        while (true)
        {
            int jobIndex;
            double x;

            std::cin >> jobIndex >> x;
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