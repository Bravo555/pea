#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <queue>
#include <chrono>
#include <random>
#include <sstream>
#include <cstring>

#include "lib.h"
#include "brute_force.cpp"
#include "dynamic_programming.cpp"
#include "branch_and_bound.cpp"
#include "satspsolver.cpp"
#include "gatspsolver.cpp"

const int INSTANCE_SIZE_MIN = 8;
const int INSTANCE_SIZE_MAX = 20;
const int REPETITIONS = 10;

std::vector<int> genRandomInstance(int numberOfCities, std::mt19937& gen) {
    std::vector<int> adjMatrix(numberOfCities * numberOfCities);
    std::uniform_int_distribution<> distribution(1, 999);
    for (int i = 0; i < numberOfCities; i++) {
        for (int j = 0; j < numberOfCities; j++) {
            if (i == j) {
                adjMatrix[i * numberOfCities + j] = -1;
            }
            else {
                adjMatrix[i * numberOfCities + j] = distribution(gen);
            }
        }
    }
    return adjMatrix;
}

void testOnFile(const std::string& filename) {
    auto time1 = std::chrono::system_clock::now();
    auto time2 = std::chrono::system_clock::now();

    Tsp tsp = Tsp::loadFromFile(filename);
    GaTspSolver gaSolver(tsp);

    time1 = std::chrono::system_clock::now();
    TspSolution tsp3 = gaSolver.solve(0, 120);
    time2 = std::chrono::system_clock::now();

    std::cout << "GENETIC ALGORITHM" << std::endl;
    std::cout << "took: " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << "ms" << std::endl;
    std::cout << "Found minimum cost: " << tsp3.cost << std::endl;
    std::cout << "order: ";
    for (auto c : tsp3.order) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

void testOnRandomData(const std::string& filename, int min, int max, int reps) {
    std::random_device rd;
    std::mt19937 gen(0);

    std::ofstream results(filename);
    results << "N,Simulated Annealing" << std::endl;

    auto testSuiteStart = std::chrono::system_clock::now();
    for (int i = min; i <= max; ++i) {
        results << i << ",";

        std::cout << "INSTANCE SIZE: " << i << std::endl;
        std::vector<std::vector<int>> instances;
        for (int rep = 0; rep < reps; ++rep) {
            std::vector<int> adjMatrix = genRandomInstance(i, gen);
            instances.emplace_back(adjMatrix);
        }

        auto start = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();

        // chosen arbitrarily
        if (i <= 12) {
            start = std::chrono::system_clock::now();
            for (auto& instance : instances) {
                tspBruteforce(instance, i);
            }
            end = std::chrono::system_clock::now();
            int bfTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / reps;
            std::cout << "\tbrute force: " << bfTime << "us" << std::endl;

            results << bfTime << ",";
        }
        else {
            results << "-1,";
        }

        start = std::chrono::system_clock::now();
        for (auto& instance : instances) {
            tspBnb(instance, i);
        }
        end = std::chrono::system_clock::now();
        int bnbTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / reps;
        std::cout << "\tbranch and bound: " << bnbTime << "us" << std::endl;
        results << bnbTime << ",";

        start = std::chrono::system_clock::now();
        for (auto& instance : instances) {
            tspDp(instance, i);
        }
        end = std::chrono::system_clock::now();
        int dpTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / reps;
        std::cout << "\tdynamic programming: " << dpTime << "us" << std::endl;
        results << dpTime << "\n";
    }

    auto testSuiteEnd = std::chrono::system_clock::now();
    std::cout << "the entire test suite took: "
        << std::chrono::duration_cast<std::chrono::seconds>(testSuiteEnd - testSuiteStart).count()
        << "s" << std::endl;
}

int main(int argc, char** argv) {
    std::random_device rd;
    std::mt19937 gen(0);

    if (argc < 2) {
        std::cout << "random OUTPUT MIN MAX REPETITIONS - generates REPETITIONS instances of sizes from MIN to MAX, "
            "solves using all the methods, and saves results to file OUTPUT" << std::endl;
        std::cout << "file PATH - loads instance from file of name PATH and prints the solution" << std::endl;
        std::cout << "q, exit - exits the program" << std::endl;

        bool exit = false;
        do {
            std::cout << "> ";

            std::string line;
            std::getline(std::cin, line);
            std::stringstream words(line);

            std::string cmd;
            words >> cmd;

            if (cmd == "random") {
                std::string filename;
                int min = 12;
                int max = 20;
                int reps = 100;
                words >> filename >> min >> max >> reps;
                testOnRandomData(filename, min, max, reps);
            }
            else if (cmd == "file") {
                std::string filename;
                words >> filename;
                testOnFile(filename);
            }
            else if (cmd == "q" || cmd == "exit")
                exit = true;
        } while (!exit);
    }

    else {
        if (std::string(argv[1]) == "random") {
            std::string filename(argv[2]);
            int min = argc >= 3 ? std::atoi(argv[2]) : 12;
            int max = argc >= 4 ? std::atoi(argv[3]) : 20;
            int reps = argc >= 5 ? std::atoi(argv[4]) : 100;
            testOnRandomData(filename, min, max, reps);
        }
        else if (std::string(argv[1]) == "file") {
            std::string filename = argv[2];
            testOnFile(filename);
        }

        return 0;
    }

}
