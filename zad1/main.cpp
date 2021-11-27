#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <queue>

#include "branch_and_bound_orig.cpp"
#include "lib.h"

int cycleDistance(const std::vector<int>& adjMatrix, int n, const std::vector<int>& order) {
    int sum = 0;
    size_t prevCity = order[0];
    for (size_t i = 1; i < order.size(); ++i) {
        sum += adjMatrix[index(prevCity, order[i], n)];
        prevCity = order[i];
    }
    return sum;
}

TspInstance tspBruteforce(const std::vector<int>& adjMatrix, int n) {
    std::vector<int> order;
    for(int i = 0; i < n; ++i) {
        order.push_back(i);
    }
    int currentMinimum = INT32_MAX;
    std::vector<int> currentMinimumOrder = order;

    bool next = false;
    do {
        int cost = cycleDistance(adjMatrix, n, order);
        if(cost < currentMinimum) {
            currentMinimum = cost;
            currentMinimumOrder = order;
        }
        next = std::next_permutation(order.begin(), order.end());
    }
    while(next && order[0] == 0);

    int i = currentMinimumOrder.back();
    currentMinimumOrder.push_back(0);
    currentMinimum += adjMatrix[index(i, 0, n)];

    TspInstance tsp(currentMinimumOrder, currentMinimum);
    return tsp;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage ./zad1 input" << std::endl;
        return -1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    int n;
    file >> n;

    std::cout << n << std::endl;

    std::vector<int> adjMatrix;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int cycleDistance;
            file >> cycleDistance;
            adjMatrix.push_back(cycleDistance);
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << adjMatrix[i * n + j] << " ";
        }
        std::cout << '\n';
    }

    TspInstance tsp1 = tspBruteforce(adjMatrix, n);
    TspInstance tsp2 = tspBnb(adjMatrix, n);

    std::cout << "Found minimum cost: " << tsp1.cost << std::endl;
    std::cout << "order: ";
    for(auto c: tsp1.order) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    std::cout << "Found minimum cost: " << tsp2.cost << std::endl;
    std::cout << "order: ";
    for(auto c: tsp2.order) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

}