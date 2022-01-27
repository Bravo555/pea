#pragma once

#include "tspsolver.cpp"
#include <random>
#include <chrono>

class SaTspSolver : public TspSolver {
public:
    SaTspSolver(const Tsp& instance) : TspSolver(instance) {}

    TspSolution solve(int start, float timeoutS) override {
        auto startTime = std::chrono::system_clock::now();
        int timeoutMs = timeoutS * 1000;

        Tsp tsp = getTsp();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> cityDist(1, tsp.size()-1);
        std::uniform_real_distribution<> realDist(0.0, 1.0);

        int n = tsp.size();

        // Define a move operation that swaps two arbitrary cities in a given solution
        auto moveSwap = [&](const std::vector<int>& order, int a, int b) {
            // don't do anything when requesting OOB swap. Maybe we should fail here.
            if(a >= order.size() - 1 || b >= order.size() - 1) {
                return order;
            }

            // if access within bounds, swap cities at indices a and b
            auto nextOrder = order;
            std::swap(nextOrder.at(a), nextOrder.at(b));
            return nextOrder;
        };

        // initialize vector to a sequence
        std::vector<int> currentOrder(tsp.size());
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
        int currentCost = tsp.cost(currentOrder);

        // find best starting solution using greedy method
        bool minimal = true;
        while(!minimal) {
            minimal = true;
            for(int a = 1; a < n-1; ++a) {
                for(int b = 2; b < n; ++b) {
                    auto nextOrder = moveSwap(currentOrder, a, b);
                    auto nextCost = tsp.cost(currentOrder);

                    if(nextCost < currentCost) {
                        currentCost = nextCost;
                        currentOrder = nextOrder;
                        minimal = false;
                    }
                }
            }
        }

        float t = 40000;
        float t_min = 0.01;
        float alpha = 0.999;
        int iterations = 0;
        int era_length = n * n;

        std::cout << "starting: ";
        printVec(currentOrder);
        std::cout << "cost: " << currentCost << std::endl;

        auto bestOrder = currentOrder;
        int bestCost = currentCost;

        int same = 0;
        int prev = 0;

        std::ofstream f("costs.csv");

        while(t > t_min) {
            auto time = std::chrono::system_clock::now();
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime).count();
            if(durationMs > timeoutMs) {
                std::cout << "aborting due to hitting timeout" <<std::endl;
                return TspSolution{bestOrder, bestCost};
            }
            if(prev == currentCost)
                ++same;
            else
                same = 0;
            prev = currentCost;

            for(int i = 0; i < 100000; ++i) {
                ++iterations;
                int a = cityDist(gen);
                int b = cityDist(gen);
                while(b == a) b = cityDist(gen);

                auto nextOrder = moveSwap(currentOrder, a, b);
                auto nextCost = tsp.cost(nextOrder);
                float prob = exp((-(nextCost - currentCost) / t));

                if(nextCost < currentCost || realDist(gen) < prob) {
                    currentCost = nextCost;
                    currentOrder = nextOrder;

                    if(currentCost < bestCost) {
                        bestCost = currentCost;
                        bestOrder = currentOrder;
                    }
                }
            }
            f << currentCost << "\n";
            t = t * alpha;

            // if stuck at the solution worse than best found so far, take the
            // best one as the current one and continue the algorithm
            if(same >= 1000 && currentCost > bestCost) {
                if(currentCost == bestCost) {
                    return TspSolution{currentOrder, currentCost};
                }
                else {
                    currentOrder = bestOrder;
                    currentCost = bestCost;
                    same = 0;
                }
            }
        }
        std::cout << "iterations: " << iterations << std::endl;

        return TspSolution{currentOrder, currentCost};
    }
};