#pragma once

#include "lib.h"

class TspSolver {
private:
    Tsp instance;

public:
    TspSolver(const Tsp& _instance) : instance(_instance) {}

    virtual TspSolution solve(int start, float timeoutS) = 0;

    const Tsp& getTsp() const {
        return instance;
    }
};