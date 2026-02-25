#include <iostream>
#include <functional>
#include <cmath>

static double Sum(std::function<double(int)> termFunc, double precision = 1e-8)
{
    double sum = 0.0;
    int n = 0;
    while (true)
    {
        double term = termFunc(n);
        if (std::abs(term) < precision)
            break;
        sum += term;
        n++;
    }
    return sum;
}

static double factorial(int n)
{
    double f = 1.0;
    for (int i = 2; i <= n; ++i)
        f *= i;
    return f;
}

int main()
{
    double precision = 1e-8;

    // 1: 1 + 1/2 + 1/4 + 1/8 + ...
    std::cout << "Geometric series sum: "
              << Sum([](int n) -> double
                     {
                         return 1.0 / (1 << n); // 1 / 2^n
                     },
                     precision)
              << "\n";

    // 2: 1 + 1/2! + 1/3! + 1/4! + ...
    std::cout << "Factorial series sum: "
              << Sum([](int n) -> double
                     {
                        if (n == 0)
                            return 1.0;
                        return 1.0 / factorial(n); },
                     precision)
              << "\n";

    // 3: -1 + 1/2 - 1/4 + 1/8 - ...
    std::cout << "Alternating series sum: "
              << Sum([](int n) -> double
                     {
                         return std::pow(-1.0, n) / (1 << n); // (-1)^n / 2^n
                     },
                     precision)
              << "\n";
}
