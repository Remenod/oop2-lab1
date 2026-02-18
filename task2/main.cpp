#include <iostream>
#include <vector>

int main() {
	std::vector<int> v = {1, 2, 3, 4, 5};
	int k = 3;

	auto filtered = v
    | std::views::filter([](int x){ return x % k == 0; });


	for (int x : filtered)
    	std::cout << x << " ";
}
