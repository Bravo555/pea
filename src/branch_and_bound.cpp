#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <queue>

#include "lib.h"

// a node used in the branch and bound solution
// we do not keep old nodes and traverse up the tree to find a solution because
// as far as i'm aware the priority queue doesn't let us do that, so we just
// incorporate old nodes content into the new nodes
struct Node {
    int cost;
    int node;
    int parent;
    int level;
    std::vector<int> adjMatrix;
    std::vector<int> order;

    Node(int _cost, int _node, int _parent, int _level, std::vector<int> _adjMatrix, std::vector<int> _order) :
        cost(_cost),
        node(_node),
        parent(_parent),
        level(_level),
        adjMatrix(_adjMatrix),
        order(_order)
    {}
};

// reduce the matrix so that each row and column has at least one '0' or has no
// valid elements (consists of all '-1's)
int reduceMatrix(std::vector<int>& adjMatrix, int n) {
    int reductionsTotal = 0;
    std::vector<int> rowMinimums(n, INT32_MAX);

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(i, j, n)] != -1 && adjMatrix[index(i, j, n)] < rowMinimums[i]) {
                rowMinimums.at(i) = adjMatrix[index(i, j, n)];
            }
        }
    }

    for(int i = 0; i < n; ++i) {
        if(rowMinimums[i] != INT32_MAX) reductionsTotal += rowMinimums[i];
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(i, j, n)] != -1 && rowMinimums[i] != INT32_MAX) {
                adjMatrix.at(index(i, j, n)) -= rowMinimums[i];
            }
        }
    }

    std::vector<int> columnMinimums(n, INT32_MAX);

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(j, i, n)] != -1 && adjMatrix[index(j, i, n)] < columnMinimums[i])
                columnMinimums.at(i) = adjMatrix[index(j, i, n)];
        }
    }

    for(int i = 0; i < n; ++i) {
        if(columnMinimums[i] != INT32_MAX) reductionsTotal += columnMinimums[i];
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(j, i, n)] != -1 && columnMinimums[i] != INT32_MAX)
                adjMatrix.at(index(j, i, n)) -= columnMinimums[i];
        }
    }

    return reductionsTotal;
}

// Finds the branch and bound solution
TspSolution tspBnb(const std::vector<int>& adjMatrix, int n) {
    int upper = INT32_MAX;

    // we initialize the components of a root node
    std::vector<int> reducedMatrix(adjMatrix);
    std::vector<int> order;
    int reduction = reduceMatrix(reducedMatrix, n);

    // https://stackoverflow.com/questions/41053232/c-stdpriority-queue-uses-the-lambda-expression
    std::priority_queue<Node, std::vector<Node>,
        decltype([](Node& lhs, Node& rhs) {
            // expand nodes that have smalles costs first, if the cost is equal,
            // prioritise deeper nodes
            return (lhs.cost > rhs.cost) || ((lhs.cost == rhs.cost) && (lhs.level < rhs.level));
        }
    )> tree;

    // -1 means lack of parent
    tree.emplace(reduction, 0, -1, 0, reducedMatrix, std::vector<int>{0});

    while(!tree.empty()) {
        Node node = tree.top();
        tree.pop();
        int i = node.node;

        // cost estimate of next-shortest path is greater than one of our
        // completed paths, solution found
        if(node.cost >= upper) {
            break;
        }

        // if level = n - 1, then we reached the leaf node, update current best
        // solution if cost is smaller than previous one
        if(node.level == n - 1 && node.cost < upper) {
            upper = node.cost;
            order = node.order;
        }

        // expand level at that node, depth first fashion
        for(int j = 0; j < n; ++j) {
            if(node.adjMatrix[index(i, j, n)] == -1) {
                continue;
            }

            // prepare a matrix with excluded row i, column j, and ji
            std::vector<int> newMatrix(node.adjMatrix);
            for(int k = 0; k < n; ++k) {
                newMatrix.at(index(i, k, n)) = -1;
                newMatrix.at(index(k, j, n)) = -1;
            }
            newMatrix.at(index(j, 0, n)) = -1;

            // reduce it
            int reduction = reduceMatrix(newMatrix, n);

            // cost of new node:
            // distance on the parent matrix + parent cost + child reduction
            int cost = node.adjMatrix[index(i, j, n)] + node.cost + reduction;

            std::vector<int> currentOrder(node.order);
            currentOrder.push_back(j);

            // put it in the tree
            tree.emplace(cost, j, i, node.level + 1, newMatrix, currentOrder);
        }
    }
    order.push_back(0);

    return TspSolution{order, upper};
}
