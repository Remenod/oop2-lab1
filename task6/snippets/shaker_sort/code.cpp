#include <iostream>
#include <vector>
using namespace std;

int main()
{
    int n;
    cin >> n;

    vector<int> arr(n);
    for (int &x : arr)
        cin >> x;

    int left = 0, right = n - 1;

    while (left < right)
    {
        for (int i = left; i < right; ++i)
            if (arr[i] > arr[i + 1])
                swap(arr[i], arr[i + 1]);

        --right;

        for (int i = right; i > left; --i)
            if (arr[i] < arr[i - 1])
                swap(arr[i], arr[i - 1]);

        ++left;
    }

    for (int x : arr)
        cout << x << " ";
}