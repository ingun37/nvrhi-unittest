//
// Created by Ingun Jon on 1/9/26.
//
#include <gtest/gtest.h>
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <vector>
// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::cout << "Original vector: ";
    for (int elem : v)
        std::cout << elem << ' ';

    auto it = std::partition(v.begin(), v.end(), [](int i) {return i % 2 == 0;});

    std::cout << "\nPartitioned vector: ";
    std::copy(std::begin(v), it, std::ostream_iterator<int>(std::cout, " "));
    std::cout << "* ";
    std::copy(it, std::end(v), std::ostream_iterator<int>(std::cout, " "));

    std::cout << std::endl;
}