#pragma once

#include <vector>
#include <numeric>

// Contains a solution to the problem
struct TspSolution {
    std::vector<int> order;
    int cost;

    TspSolution(std::vector<int> _order, int _cost) :
        order(_order), cost(_cost) {}
};

// the function transforms the index of a two-dimensional matrix to an index
// of a one-dimensional vector that stores rows of the matrix in-order.
int index(int y, int x, int n) {
    return y * n + x;
}

class Tsp {
    std::vector<int> adjMatrix;
    int n;

public:
    Tsp(const std::vector<int>& _adjMatrix, int _n) : adjMatrix(std::move(_adjMatrix)), n(_n) {}

    size_t size() const { return n; }

    int& get(size_t x, size_t y) {
        return adjMatrix.at(y * n + x);
    }

    const int& get(size_t x, size_t y) const {
        return adjMatrix.at(y * n + x);
    }

    int cost(const std::vector<int>& order) const {
        int sum = 0;
        size_t prevCity = order.at(0);
        for (size_t i = 1; i < order.size(); ++i) {
            sum += get(prevCity, order[i]);
            prevCity = order.at(i);
        }
        return sum;
    }
};

template <typename T>
void printVec(const std::vector<T>& vec) {
    for(auto& v: vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}