n = int(input())
arr = list(map(int, input().split()))
for i in range(n):
    m = i
    for j in range(i + 1, n):
        if arr[j] < arr[m]: 
            m = j
    arr[i], arr[m] = arr[m], arr[i]
print(' '.join(map(str, arr)))