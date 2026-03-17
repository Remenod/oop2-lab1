#include <iostream>
#include <vector>

int main()
{
    int n;
    std::cin >> n;

    std::vector<int> arr(n);
    for (auto &x : arr)
        std::cin >> x;

    for (int i = 0; i < n - 1; ++i)
    {
        int m = i;
        for (int j = i + 1; j < n; ++j)
            if (arr[j] < arr[m])
                m = j;

        std::swap(arr[i], arr[m]);
    }

    for (const auto &x : arr)
        std::cout << x << ' ';
}