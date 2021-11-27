#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <queue>
#include "lib.h"

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

int reduceMatrix(std::vector<int>& adjMatrix, int n) {
    int reductionsTotal = 0;
    std::vector<int> rowMinimums(n, INT32_MAX);

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(i, j, n)] != -1 && adjMatrix[index(i, j, n)] < rowMinimums[i]) {
                rowMinimums[i] = adjMatrix[index(i, j, n)];
            }
        }
    }

    for(int i = 0; i < n; ++i) {
        if(rowMinimums[i] != INT32_MAX) reductionsTotal += rowMinimums[i];
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(i, j, n)] != -1 && rowMinimums[i] != INT32_MAX) {
                adjMatrix[index(i, j, n)] -= rowMinimums[i];
            }
        }
    }

    std::vector<int> columnMinimums(n, INT32_MAX);

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(j, i, n)] != -1 && adjMatrix[index(j, i, n)] < columnMinimums[i])
                columnMinimums[i] = adjMatrix[index(j, i, n)];
        }
    }

    for(int i = 0; i < n; ++i) {
        if(columnMinimums[i] != INT32_MAX) reductionsTotal += columnMinimums[i];
        for(int j = 0; j < n; ++j) {
            if(adjMatrix[index(j, i, n)] != -1 && columnMinimums[i] != INT32_MAX)
                adjMatrix[index(j, i, n)] -= columnMinimums[i];
        }
    }

    return reductionsTotal;
}

TspInstance tspBnb(const std::vector<int> adjMatrix, int n) {
    int upper = INT32_MAX;
    std::vector<int> reducedMatrix(adjMatrix);
    std::vector<int> order;
    int reduction = reduceMatrix(reducedMatrix, n);

    // https://stackoverflow.com/questions/41053232/c-stdpriority-queue-uses-the-lambda-expression
    std::priority_queue<Node, std::vector<Node>,
        decltype([](Node& lhs, Node& rhs) { return lhs.cost > rhs.cost; }
    )> tree;

    // -1 means lack of parent
    tree.emplace(reduction, 0, -1, 0, reducedMatrix, std::vector<int>{0});

    while(!tree.empty()) {
        Node node = tree.top();
        tree.pop();
        int i = node.node;

        // cost estimate of next-shortest path is greater than one of our
        // completed paths, solution found
        if(node.cost > upper) {
            break;
        }

        if(node.level == n - 1 && node.cost < upper) {
            upper = node.cost;
            order = node.order;
        }

        // expand level at that node
        for(int j = 0; j < n; ++j) {
            if(node.adjMatrix[index(i, j, n)] == -1) {
                continue;
            }

            // prepare a matrix with excluded row i, column j, and ji
            std::vector<int> newMatrix(node.adjMatrix);
            for(int k = 0; k < n; ++k) {
                newMatrix[index(i, k, n)] = -1;
                newMatrix[index(k, j, n)] = -1;
            }
            newMatrix[index(j, 0, n)] = -1;
            std::cout << "Node: " << i << "->" << j << std::endl;
            for(int i = 0; i < n; ++i) {
                std::cout << "\t";
                for(int j = 0; j < n; ++j) {
                    std::cout << newMatrix[index(i, j, n)] << "\t";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            // reduce it
            int reduction = reduceMatrix(newMatrix, n);

            std::cout << "Node after: " << i << "->" << j << std::endl;
            for(int i = 0; i < n; ++i) {
                std::cout << "\t";
                for(int j = 0; j < n; ++j) {
                    std::cout << newMatrix[index(i, j, n)] << "\t";
                }
                std::cout << std::endl;
            }

            // cost of new node:
            // distance on the parent matrix + parent cost + child reduction
            int cost = node.adjMatrix[index(i, j, n)] + node.cost + reduction;
            std::cout << node.adjMatrix[index(i, j, n)] << " "
                << node.cost << " " << reduction << std::endl;
            std::cout << "cost: " << cost << std::endl;
            std::cout << std::endl;

            std::vector<int> currentOrder(node.order);
            currentOrder.push_back(j);

            // put it in the tree
            tree.emplace(cost, j, i, node.level + 1, newMatrix, currentOrder);
        }
    }

    // while(!tree.empty()) {
    //     Node node = tree.top();
    //     tree.pop();
    //     std::cout << "\tnode " << node.node << ", "
    //         << "cost " << node.cost << " "
    //         << "parent " << node.parent << " "
    //         << "level " << node.level << std::endl;

    //     for(int i = 0; i < n; ++i) {
    //         std::cout << "\t";
    //         for(int j = 0; j < n; ++j) {
    //             std::cout << "\t" << node.adjMatrix[index(i, j, n)] << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

    order.push_back(0);

    return TspInstance{order, upper};
}
