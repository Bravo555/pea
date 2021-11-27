#pragma once

#include <vector>
#include <numeric>

struct TspInstance {
    std::vector<int> order;
    int cost;

    TspInstance(std::vector<int> _order, int _cost) :
        order(_order), cost(_cost) {}
};

int index(int y, int x, int n) {
    return y * n + x;
}