#include <iostream>
#include <vector>
#include <functional>

std::vector<int> Where(const std::vector<int>& arr, std::function<bool(int)> pred) {
    std::vector<int> result;
    for (int x : arr) {
        if (pred(x))
            result.push_back(x);
    }
    return result;
}

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
    std::cout << "Enter Arr Elements count: ";
    std::cin >> n;

    int* arr = new int[n];

    std::cout << "Enter " << n << " numbers:\n";
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    std::function<bool(int)> pred = [k](int x){ return x % k == 0; };

    // lib method
    std::vector<int> filtered = Where({arr, arr+n}, pred);
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
