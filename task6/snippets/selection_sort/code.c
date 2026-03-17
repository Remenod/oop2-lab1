#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n;
    scanf("%d", &n);

    int *arr = malloc(n * sizeof(int));

    for (int i = 0; i < n; ++i)
        scanf("%d", &arr[i]);

    for (int i = 0; i < n - 1; ++i)
    {
        int m = i;
        for (int j = i + 1; j < n; ++j)
            if (arr[j] < arr[m])
                m = j;

        int t = arr[i];
        arr[i] = arr[m];
        arr[m] = t;
    }

    for (int i = 0; i < n; ++i)
        printf("%d ", arr[i]);

    free(arr);
    return 0;
}