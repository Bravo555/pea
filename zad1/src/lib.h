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