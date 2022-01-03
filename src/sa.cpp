#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <fstream>

#include "lib.h"

// Solves traveling salesman problem using tabu search
TspSolution tspSa(const Tsp& tsp, int start) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> cityDist(0, tsp.size()-2);
    std::uniform_real_distribution<> realDist(0.0, 1.0);

    float t = 300.0;
    float t_min = 0.1;
    float alpha = 0.999;
    int iterations = 0;

    // Define a move operation that swaps two adjacent cities in a given solution
    // TODO: make it copy-free
    auto moveSwap = [&](const std::vector<int>& order, int a, int b) {
        a += 1;
        b += 1;

        // don't do anything when requesting OOB swap. Maybe we should fail here.
        if(a >= order.size() - 1 || b >= order.size() - 1) {
            return order;
        }

        // if access within bounds, swap cities at indices a and b
        auto nextOrder = order;
        std::swap(nextOrder.at(a), nextOrder.at(b));
        return nextOrder;
    };

    auto moveSwapAdj = [&](const std::vector<int>& order, int a) {
        a += 1;
        // don't do anything when requesting OOB swap. Maybe we should fail here.
        if(a >= order.size() - 2) {
            return order;
        }

        // if access within bounds, swap cities at indices a and a+1
        auto nextOrder = order;
        std::swap(nextOrder.at(a), nextOrder.at(a+1));
        return nextOrder;
    };

    // initialize vector to a sequence
    std::vector<int> currentOrder(tsp.size());
    auto bestOrder = currentOrder;
    currentOrder.at(0) = start;
    {
        int val = 0;
        for(auto c = currentOrder.begin()+1; c != currentOrder.end(); ++c) {
            if(val == start) {
                val++;
            }
            *c = val++;
        }
    }

    // shuffle the sequence after start city
    std::shuffle(currentOrder.begin() + 1, currentOrder.end(), gen);
    currentOrder.push_back(0);

    int cost = tsp.cost(currentOrder);

    std::cout << "starting:" << std::endl;
    printVec(currentOrder);
    std::cout << "cost: " << cost << std::endl;

    int currentCost = cost;
    int bestCost = cost;

    int same = 0;
    int prev = 0;

    std::ofstream f("costs.csv");
    std::ofstream path("path.txt");
    std::ofstream change("change.txt");

    while(t > t_min) {
        if(prev == currentCost)
            ++same;
        else
            same = 0;
        prev = currentCost;

        for(int i = 0; i < 10; ++i) {
            ++iterations;
            int a = cityDist(gen);
            int b = cityDist(gen);
            while(b == a) b = cityDist(gen);

            auto nextOrder = moveSwap(currentOrder, a, b);
            auto nextCost = tsp.cost(nextOrder);
            float prob = exp((-(nextCost - currentCost) / t));
            // std::cout << "changed: " << a << " " << " " << b << "\n";
            // std::cout << "cost: " << bestCost << " ";
            // std::cout << "->" << nextCost << "\t";
            // std::cout << prob << std::endl;


            if(nextCost < currentCost || realDist(gen) < prob) {
                currentCost = nextCost;
                currentOrder = nextOrder;

                if(currentCost < bestCost) {
                    bestCost = currentCost;
                    bestOrder = currentOrder;
                }

                for(auto& c: currentOrder) {
                    path << c << " ";
                }
                path << "\n";
            }
            f << currentCost << "\n";
        }
        t = t * alpha;

        if(same >= 50 && currentCost > bestCost) {
            currentOrder = bestOrder;
            currentCost = bestCost;
            same = 0;
        }
    }
    std::cout << "iterations: " << iterations << std::endl;
    std::cout << "best cost: " << bestCost << std::endl;

    return TspSolution{currentOrder, currentCost};
}