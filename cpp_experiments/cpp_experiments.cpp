#include "vector/vector.h"
#include <iostream>
#include <algorithm>

int main() {
	vector<int> test_init_list = { 1, 2, 3, 4, 6, 7, 9 };

	auto place = std::find(test_init_list.begin(), test_init_list.end(), 6);
	test_init_list.emplace(place, 5);

	place = std::find(test_init_list.begin(), test_init_list.end(), 9);
	test_init_list.emplace(place, 8);

	for (const auto& it : test_init_list) {
		std::cout << it << std::endl;
	}
	return 0;
}
