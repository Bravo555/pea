#pragma once

#include <vector>
#include <cstdint>

#include "lib.h"

// The function calculates the distance of the path for a given adjecency matrix
int cycleDistance(const std::vector<int>& adjMatrix, int n, const std::vector<int>& order) {
    int sum = 0;
    size_t prevCity = order[0];
    for (size_t i = 1; i < order.size(); ++i) {
        sum += adjMatrix[index(prevCity, order[i], n)];
        prevCity = order[i];
    }
    return sum;
}

// Calculates and returns a solution using the brute force method.
// The path starts from city 0
TspSolution tspBruteforce(const std::vector<int>& adjMatrix, int n) {
    std::vector<int> order;
    for(int i = 0; i < n; ++i) {
        order.push_back(i);
    }
    int currentMinimum = INT32_MAX;
    std::vector<int> currentMinimumOrder = order;

    bool next = false;
    do {
        int cost = cycleDistance(adjMatrix, n, order);
        // if current path is smaller than the minimum, update the minimum
        if(cost < currentMinimum) {
            currentMinimum = cost;
            currentMinimumOrder = order;
        }
        // Given a permutation, this function returns a next permutation
        // next permutation is greater lexicographically than the previous one
        // That is, for n = 4, the order of the permutation is:
        // 1234 1243 1324 1342 1423 1432
        // 2134 2143 2314 2341 2413 2431
        // 3124 3142 3214 3241 3412 3421
        // 4123 4132 4213 4231 4312 4321
        next = std::next_permutation(order.begin(), order.end());
    }
    while(next && order[0] == 0);

    int i = currentMinimumOrder.back();
    currentMinimumOrder.push_back(0);
    currentMinimum += adjMatrix[index(i, 0, n)];

    TspSolution tsp{currentMinimumOrder, currentMinimum};
    return tsp;
}