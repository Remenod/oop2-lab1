#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n;
    scanf("%d", &n);

    int *arr = malloc(n * sizeof(int));

    for (int i = 0; i < n; ++i)
        scanf("%d", &arr[i]);

    int left = 0, right = n - 1;

    while (left < right)
    {
        for (int i = left; i < right; ++i)
            if (arr[i] > arr[i + 1])
            {
                int t = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = t;
            }

        --right;

        for (int i = right; i > left; --i)
            if (arr[i] < arr[i - 1])
            {
                int t = arr[i];
                arr[i] = arr[i - 1];
                arr[i - 1] = t;
            }

        ++left;
    }

    for (int i = 0; i < n; ++i)
        printf("%d ", arr[i]);

    free(arr);
}