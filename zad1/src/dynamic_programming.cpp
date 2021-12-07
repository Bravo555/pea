#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <cstdint>

#include "lib.h"

void generateSetsA(int set, int at, int k, int n, std::unordered_set<int>& sets) {
    if(k == 0) {
        sets.insert(set);
        return;
    }

    for(int i = at; i < n; ++i) {
        // generate a combination with i-th bit set
        set |= (1 << i);
        generateSetsA(set, i + 1, k - 1, n, sets);

        // flip the bit off. in the next iteration of the loop we will be
        // considering next bits, and as such, the combinations without now i-th
        // bit set will be generated
        set &= ~(1 << i);
    }
}

// function generates all sets for n number of cities with k cities visited
// the type of the topmost set is int because int is used as a bitfield
// indicating which cities have been visited, where the bit position is the
// index of the city visited.
std::unordered_set<int> generateSets(int k, int n) {
    std::unordered_set<int> sets;
    generateSetsA(0, 0, k, n, sets);
    return sets;
}

// we check if the set contains the city given by the index `node`, by checking
// whether the bit at position `node` is set
bool setContains(int set, int node) {
    return ((1 << node) & set) != 0;
}

// calculates the solution using the dynamic programming approach
TspSolution tspDp(const std::vector<int>& adjMatrix, const int n) {
    int start = 0;

    // A 2D array that:
    // in the 1st dimension has city index, so for each city:
    // in the 2nd dimension has all the bitfield sets which designate a path
    // to reach a given city, so for each path:
    // we finally store the cost to reach the node in the 1st dimension, by the
    // path in the 2nd dimension
    std::vector<std::vector<int>> distances(n, std::vector<int>(1 << n, 0));

    // start by initializing all the direct paths from start node to all the
    // other nodes
    for(int i = 0; i < n; ++i) {
        if(i == start) continue;

        distances[i][1 << start | 1 << i] = adjMatrix[index(start, i, n)];
    }

    // for all sets of size 3 up to n
    for(int k = 3; k <= n; ++k) {
        // for each set of size k
        std::unordered_set<int> sets = generateSets(k, n);
        for(auto& set: sets) {
            // set has to contain starting node
            if(!setContains(set, start)) continue;

            for(int next = 0; next < n; ++next) {
                if(next == start || !setContains(set, next)) continue;

                int currentPath = set & ~(1 << next);
                int minDistance = INT32_MAX;

                for(int end = 0; end < n; ++end) {
                    if(end == start || end == next || !setContains(set, end))
                        continue;
                    
                    int newDistance = distances[end][currentPath]
                        + adjMatrix[index(end, next, n)];
                    if(newDistance < minDistance) minDistance = newDistance;
                }
                distances[next][set] = minDistance;
            }
        }
    }

    // calculate the cost of the shortest path

    // end state is all nodes visited, so all nodes up to n set
    int endState = (1 << n) - 1;
    int minTourCost = INT32_MAX;

    for(int end = 0; end < n; ++end) {
        if(end == start) continue;

        // we build final paths such as path ends at node `end`, for all nodes,
        // and we add edge "end -> start" at the end to complete the cycle
        int tourCost = distances[end][endState]
            + adjMatrix[index(end, start, n)];

        if(tourCost < minTourCost) minTourCost = tourCost;
    }

    // construct the shortest path

    int lastIndex = start;
    std::vector<int> order(n + 1);
    int state = endState;

    // walk backwards from the end state, to find the previous node
    for(int i = n - 1; i >= 1; --i) {
        int bestIndex = -1;

        // starting from the end of the cycle (starting node) t = s we find
        // previous node p such as path: s -> ... -> p -> t is the shortest.
        // once we find it, we set t = p and repeat until we have reached the
        // starting node again
        for(int j = 0; j < n; ++j) {
            if(j == start || !setContains(state, j)) continue;

            if(bestIndex == -1) bestIndex = j;

            int prevDist = distances[bestIndex][state]
                + adjMatrix[index(bestIndex, lastIndex, n)];

            int newDist = distances[j][state]
                + adjMatrix[index(j, lastIndex, n)];

            // if path s -> ... -> j -> lastIndex is shorter, update bestIndex
            if(newDist < prevDist) bestIndex = j;
        }

        order.at(i) = bestIndex;
        state &= ~(1 << bestIndex);
        lastIndex = bestIndex;
    }

    order.at(0) = start;
    order.at(n) = start;

    return TspSolution{order, minTourCost};
}
