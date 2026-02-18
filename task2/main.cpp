#include <iostream>
#include <vector>
#include <ranges>
#include <functional>

int* WhereManual(int* arr, int length, std::function<bool(int)> pred, int& outLength) {
    int count = 0;
    for (int i = 0; i < length; ++i)
        if (pred(arr[i]))
            ++count;

    outLength = count;
    int* result = new int[count];

    int idx = 0;
    for (int i = 0; i < length; ++i)
        if (pred(arr[i]))
            result[idx++] = arr[i];

    return result;
}

int main() {
    int k;
    std::cout << "Enter k: ";
    std::cin >> k;

    int n;
    std::cout << "Enter Arr len: ";
    std::cin >> n;

    int* arr = new int[n];

    std::cout << "Enter " << n << " numbers:\n";
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    std::function<bool(int)> pred = [k](int x){ return x % k == 0; };

    // lib method
	auto filteredView = std::ranges::subrange(arr, arr + n)
                    | std::views::filter(pred);

	std::vector<int> filtered(filteredView.begin(), filteredView.end());

    std::cout << "Filtered with lib: ";
    for (int x : filtered) std::cout << x << " ";
    std::cout << "\n";

    // manual method
    int manualLength;
    int* manualResult = WhereManual(arr, n, pred, manualLength);
    std::cout << "Filtered manual: ";
    for (int i = 0; i < manualLength; ++i)
        std::cout << manualResult[i] << " ";
    std::cout << "\n";

    delete[] manualResult;
    delete[] arr;
}
