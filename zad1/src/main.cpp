#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <queue>
#include <chrono>
#include <random>
#include <sstream>

#include "lib.h"
#include "brute_force.cpp"
#include "dynamic_programming.cpp"
#include "branch_and_bound.cpp"

const int INSTANCE_SIZE_MIN = 8;
const int INSTANCE_SIZE_MAX = 22;
const int REPETITIONS = 100;

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
    std::ifstream file(filename);
    int n;
    file >> n;

    std::cout << n << std::endl;

    std::vector<int> adjMatrix;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int cycleDistance;
            file >> cycleDistance;
            adjMatrix.push_back(cycleDistance);
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << adjMatrix[i * n + j] << " ";
        }
        std::cout << '\n';
    }

    auto time1 = std::chrono::system_clock::now();
    auto time2 = std::chrono::system_clock::now();

    time1 = std::chrono::system_clock::now();
    TspSolution tsp1 = tspBruteforce(adjMatrix, n);
    time2 = std::chrono::system_clock::now();

    std::cout << "BRUTE FORCE:" << std::endl;
    std::cout << "took: " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << "ms" <<  std::endl;
    std::cout << "Found minimum cost: " << tsp1.cost << std::endl;
    std::cout << "order: ";
    for(auto c: tsp1.order) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    time1 = std::chrono::system_clock::now();
    TspSolution tsp2 = tspBnb(adjMatrix, n);
    time2 = std::chrono::system_clock::now();

    std::cout << "BRANCH AND BOUND:" << std::endl;
    std::cout << "took: " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << "ms" << std::endl;
    std::cout << "Found minimum cost: " << tsp2.cost << std::endl;
    std::cout << "order: ";
    for(auto c: tsp2.order) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    time1 = std::chrono::system_clock::now();
    TspSolution tsp3 = tspDp(adjMatrix, n);
    time2 = std::chrono::system_clock::now();

    std::cout << "DYNAMIC PROGRAMMING:" << std::endl;
    std::cout << "took: " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << "ms" << std::endl;
    std::cout << "Found minimum cost: " << tsp3.cost << std::endl;
    std::cout << "order: ";
    for(auto c: tsp3.order) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

void testOnRandomData(const std::string& filename) {
    std::random_device rd;
    std::mt19937 gen(0);

    std::ofstream results(filename);
    results << "N,Brute force,Branch and Bound,Dynamic Programming" << std::endl;

    auto testSuiteStart = std::chrono::system_clock::now();
    for(int i = INSTANCE_SIZE_MIN; i <= INSTANCE_SIZE_MAX; ++i) {
        results << i << ",";

        std::cout << "INSTANCE SIZE: " << i << std::endl;
        std::vector<std::vector<int>> instances;
        for(int rep = 0; rep < REPETITIONS; ++rep) {
            std::vector<int> adjMatrix = genRandomInstance(i, gen);
            instances.emplace_back(adjMatrix);
        }

        auto start = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();

        if(i <= 12) {
            start = std::chrono::system_clock::now();
            for(auto& instance: instances) {
                tspBruteforce(instance, i);
            }
            end = std::chrono::system_clock::now();
            int bfTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / REPETITIONS;
            std::cout << "\tbrute force: " << bfTime << "ms" << std::endl;

            results << bfTime << ",";
        }
        else {
            results << "-1,";
        }

        start = std::chrono::system_clock::now();
        for(auto& instance: instances) {
            tspBnb(instance, i);
        }
        end = std::chrono::system_clock::now();
        int bnbTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / REPETITIONS;
        std::cout << "\tbranch and bound: " << bnbTime << "ms" << std::endl;
        results << bnbTime << ",";

        start = std::chrono::system_clock::now();
        for(auto& instance: instances) {
            tspDp(instance, i);
        }
        end = std::chrono::system_clock::now();
        int dpTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / REPETITIONS;
        std::cout << "\tdynamic programming: " << dpTime << "ms" << std::endl;
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
    // std::vector<int> instance = genRandomInstance(24, gen);
    // TspSolution tDp = tspDp(instance, 24);
    // TspSolution tBnb = tspBnb(instance, 24);


    // for(auto c: tDp.order) {
    //     std::cout << c << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "cost dp: " << tDp.cost << std::endl;

    // for(auto c: tBnb.order) {
    //     std::cout << c << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "cost bnb: " << tBnb.cost << std::endl;

    // return 0;

    if (argc < 2) {
        std::cout << "random OUTPUT - generates instances and saves results to file OUTPUT" << std::endl;
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

            if(cmd == "random") {
                std::string filename;
                words >> filename;
                testOnRandomData(filename);
            }
            else if(cmd == "file") {
                std::string filename;
                words >> filename;
                testOnFile(filename);
            }
            else if(cmd == "q" || cmd == "exit")
                exit = true;
        } while(!exit);
    }

    else {
        std::cout << argv[1] << std::endl;
        if(std::string(argv[1]) == "random") {
            std::string filename(argv[2]);
            testOnRandomData(filename);
        }
        else if(std::string(argv[1]) == "file") {
            std::string filename = argv[2];
            testOnFile(filename);
        }

        return 0;
    }

}