#pragma once

#include <chrono>
#include <random>

#include "tspsolver.cpp"
#include "lib.h"

namespace chrono = std::chrono;

class GaTspSolver : public TspSolver {
private:
    struct Chromosome {
        std::vector<int> order;
        std::vector<int> pathCost;
        std::vector<int> notUsed;
        int cost = 0;
    };

    struct GaParameters {
        double mutationFactor, crossoverFactor;
        int generations, population_size;
    } parameters;

    int bestCost = INT32_MAX;
    std::vector<int> bestFoundPath;
    int iterations = 0;
    long int tookTime = 0;

public:
    GaTspSolver(const Tsp& instance) : TspSolver(instance) {
        parameters.crossoverFactor = 0.8;
        parameters.mutationFactor = 0.01;
        parameters.population_size = 64;
        parameters.generations = 10000000;
    }

    TspSolution solve(int startCity, float timeoutS) override {
        Tsp tsp = getTsp();
        std::vector<int> adjMatrix = tsp.getAdjMatrix();
        int citiesNumber = tsp.size();

        auto start = chrono::high_resolution_clock::now();
        auto end = chrono::high_resolution_clock::now();
        auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start);
        float timeS;

        std::ofstream f("costs.csv");

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> realDist(0.0, 1.0);
        std::uniform_int_distribution<> randGenome(0, parameters.population_size - 1);

        std::vector<Chromosome> population;
        Chromosome tempGenome;
        int generation = 1;

        for (int i = 0; i < parameters.population_size; i++) {
            tempGenome = generateSolution(adjMatrix, citiesNumber);
            population.push_back(tempGenome);
        }

        do {
            std::vector<Chromosome> newPopulation;

            for (int i = 0; i < population.size(); i++) {
                f << population[i].cost << "\n";
            }

            for (int i = 0; i < parameters.population_size / 2; i++) {
                iterations++;
                Chromosome parent1 = selectParent(population);
                Chromosome parent2 = selectParent(population);
                Chromosome child1;
                Chromosome child2;

                if (realDist(gen) <= parameters.crossoverFactor) {
                    child1 = crossover(parent1, parent2, citiesNumber);
                    child2 = crossover(parent2, parent1, citiesNumber);
                }
                else {
                    child1 = parent1;
                    child2 = parent2;
                }

                if (realDist(gen) <= parameters.mutationFactor) {
                    child1 = transpositionMutation(child1, citiesNumber);
                    child2 = transpositionMutation(child2, citiesNumber);
                }

                child1.cost = calculateCost(adjMatrix, child1, citiesNumber);
                child2.cost = calculateCost(adjMatrix, child2, citiesNumber);

                newPopulation.push_back(child1);
                newPopulation.push_back(child2);

                if (bestCost > child1.cost) {
                    bestFoundPath = child1.order;
                    bestCost = child1.cost;
                }
                if (bestCost > child2.cost) {
                    bestFoundPath = child2.order;
                    bestCost = child2.cost;
                }
            }
            population = newPopulation;
            generation++;
            end = std::chrono::high_resolution_clock::now();
            time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            timeS = (float)time.count() / 1000.0;
        } while (timeS < timeoutS && generation < parameters.generations);
        tookTime = time.count();

        return TspSolution{bestFoundPath, bestCost};
    }

    int calculateCost(std::vector<int> adjMatrix, Chromosome& solution, int citiesNumber) {
        int cost = 0;
        solution.pathCost[citiesNumber - 1] = adjMatrix[solution.order[citiesNumber - 1] * citiesNumber + solution.order[0]];
        cost += solution.pathCost[citiesNumber - 1];
        for (int i = 0; i < citiesNumber - 1; i++) {
            solution.pathCost[i] = adjMatrix[solution.order[i] * citiesNumber + solution.order[i + 1]];
            cost += solution.pathCost[i];
        }
        return cost;
    }

    Chromosome selectParent(std::vector<Chromosome> population) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> realDist(0.0, 1.0);
        int bestIndex = 0, worstInPop = 0, bestInPop = INT32_MAX;
        for (int i = 0; i < population.size(); i++) {
            if (population[i].cost < bestInPop) {
                bestIndex = i;
                bestInPop = population[i].cost;
            }
            if (population[i].cost > worstInPop) {
                worstInPop = population[i].cost;
            }
        }
        int diff = worstInPop - bestInPop;
        for (int i = 0; i < population.size(); i++) {
            double selectionChance = ((double)worstInPop - population[i].cost) / (double)diff;
            if (selectionChance >= 1.0) {
                selectionChance -= 0.1;
            }
            if (realDist(gen) <= selectionChance) {
                return population[i];
            }
        }
        return population[bestIndex];
    }

    Chromosome crossover(Chromosome parent1, Chromosome parent2, int citiesNumber) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> randCity(citiesNumber / 4, citiesNumber - (citiesNumber / 4));
        Chromosome child;
        int breakpoint = randCity(gen);
        int temp = 0;
        for (int i = 0; i < breakpoint; i++) {
            temp = parent1.order[i];
            for (int j = 0; j < parent2.order.size(); j++) {
                if (temp == parent2.order[j]) {
                    parent2.order[j] = parent2.order[i];
                    parent2.order[i] = temp;
                }
            }
        }
        child = parent2;
        return child;
    }

    Chromosome transpositionMutation(Chromosome solution, int citiesNumber) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> randCity(0, citiesNumber - 1);
        int randIndex1 = randCity(gen);
        int randIndex2 = randCity(gen);
        while (randIndex1 == randIndex2)
            randIndex2 = randCity(gen);
        int temp = solution.order[randIndex1];
        solution.order[randIndex1] = solution.order[randIndex2];
        solution.order[randIndex2] = temp;
        return solution;
    }

    Chromosome insertionMutation(Chromosome solution, int citiesNumber) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> randCity(0, citiesNumber - 1);
        int randIndex1 = randCity(gen);
        int randIndex2 = randCity(gen);
        while (randIndex1 == randIndex2)
            randIndex2 = randCity(gen);
        int temp = solution.order[randIndex1];
        solution.order.erase(solution.order.begin() + randIndex1);
        solution.order.insert(solution.order.begin() + randIndex2, temp);
        return solution;
    }

    Chromosome generateSolution(std::vector<int> adjMatrix, int citiesNumber) {
        Chromosome genome;
        genome.order.resize(citiesNumber);
        genome.pathCost.resize(citiesNumber);
        genome.order[0] = 0;
        int temp = 0, randIndex;
        std::random_device rd;
        std::mt19937 gen(rd());

        for (int i = 0; i < citiesNumber - 1; i++) {
            bool isUsed = false;
            genome.notUsed.clear();
            // check which cities have not been visited yet
            for (int k = 0; k < citiesNumber; k++) {
                isUsed = false;
                for (int j = 0; j < genome.order.size(); j++) {
                    if (genome.order.at(j) == k) {
                        isUsed = true;
                        break;
                    }
                }
                if (!isUsed) {
                    genome.notUsed.push_back(k);
                }
            }

            // Wybieranie sciezki losowo
            std::uniform_int_distribution<> randCity(0, genome.notUsed.size() - 1);
            randIndex = randCity(gen);
            temp = adjMatrix.at(genome.order.at(i) * citiesNumber + genome.notUsed.at(randIndex));
            genome.order.at(i + 1) = genome.notUsed.at(randIndex);
            genome.pathCost.at(i) = temp;
            genome.cost += genome.pathCost.at(i);
        }
        genome.pathCost.at(citiesNumber - 1) = adjMatrix.at(genome.order.at(citiesNumber - 1) * citiesNumber + genome.order.at(0));
        genome.cost += genome.pathCost.at(citiesNumber - 1);
        if (bestCost > genome.cost) {
            bestFoundPath = genome.order;
            bestCost = genome.cost;
        }

        return genome;
    }
};
