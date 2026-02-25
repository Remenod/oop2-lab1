#include <iostream>
#include <vector>
#include <ranges>
#include <functional>

std::vector<int> WhereManual(
    const std::vector<int> &arr,
    const std::function<bool(int)> &pred)
{
    std::vector<int> result;
    result.reserve(arr.size());

    for (int x : arr)
        if (pred(x))
            result.push_back(x);

    return result;
}

int main()
{
    int k;
    std::cout << "Enter k: ";
    std::cin >> k;

    int n;
    std::cout << "Enter Arr len: ";
    std::cin >> n;

    std::vector<int> arr(n);
    std::cout << "Enter " << n << " numbers:\n";
    for (int &x : arr)
        std::cin >> x;

    std::function<bool(int)> pred = [k](int x)
    {
        return x % k == 0;
    };

    // lib method
    auto filteredView = arr | std::views::filter(pred);
    std::vector<int> filtered(filteredView.begin(), filteredView.end());

    std::cout << "Filtered with lib: ";
    for (int x : filtered)
        std::cout << x << " ";
    std::cout << "\n";

    // manual method
    auto manualResult = WhereManual(arr, pred);

    std::cout << "Filtered manual: ";
    for (int x : manualResult)
        std::cout << x << " ";
    std::cout << "\n";
}
