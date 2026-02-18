#include <iostream>
#include <functional>
#include <cmath>

class SeriesCalculator
{
public:
    using TermFunction = std::function<double(int)>;

    static double Sum(TermFunction termFunc, double precision = 1e-8)
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
};

double factorial(int n)
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
    auto geometric = [](int n) -> double
    {
        return 1.0 / (1 << n); // 1 / 2^n
    };

    // 2: 1 + 1/2! + 1/3! + 1/4! + ...
    auto factorialSeries = [](int n) -> double
    {
        if (n == 0)
            return 1.0;
        return 1.0 / factorial(n);
    };

    // 3: -1 + 1/2 - 1/4 + 1/8 - ...
    auto alternating = [](int n) -> double
    {
        return std::pow(-1.0, n) / (1 << n); // (-1)^n / 2^n
    };

    std::cout << "Geometric series sum: "
              << SeriesCalculator::Sum(geometric, precision) << "\n";

    std::cout << "Factorial series sum: "
              << SeriesCalculator::Sum(factorialSeries, precision) << "\n";

    std::cout << "Alternating series sum: "
              << SeriesCalculator::Sum(alternating, precision) << "\n";
}
