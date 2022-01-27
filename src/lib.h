#pragma once

#include <vector>
#include <numeric>
#include <fstream>
#include <cassert>
#include <sstream>
#include <iostream>

// Contains a solution to the problem
struct TspSolution {
    std::vector<int> order;
    int cost;

    TspSolution(std::vector<int> _order, int _cost) :
        order(_order), cost(_cost) {}
};

// the function transforms the index of a two-dimensional matrix to an index
// of a one-dimensional vector that stores rows of the matrix in-order.
int index(int y, int x, int n) {
    return y * n + x;
}

template <typename T>
void printVec(const std::vector<T>& vec) {
    for(auto& v: vec) {
        std::cout << v << " ";
    }
    std::cout << "\n";
}

class Tsp {
    std::vector<int> adjMatrix;
    int n;

public:
    Tsp(const std::vector<int>& _adjMatrix, int _n) : adjMatrix(std::move(_adjMatrix)), n(_n) {}

    static Tsp loadFromFile(const std::string& filename) {
        if(filename.substr(filename.find_last_of(".") + 1) == "atsp") {
            return loadFromAtsp(filename);
        } else {
            return loadFromTxt(filename);
        }
    }

    static Tsp loadFromAtsp(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::stringstream ss;
        std::string s;
        int n = 0;

        while(line != "EDGE_WEIGHT_SECTION") {
            std::getline(file, line);

            ss.str("");
            ss << line;
            ss >> s;
            if(s == "DIMENSION:") {
                ss >> n;
            }
        }

        std::vector<int> adjMatrix;
        while(true) {
            file >> s;
            if(s == "EOF")  break;

            adjMatrix.push_back(std::stoi(s));
        }

        return Tsp{adjMatrix, n};
    }

    static Tsp loadFromTxt(const std::string& filename) {
        int n;
        std::vector<int> adjMatrix;

        std::ifstream file;
        try {
            file.open(filename);
            file.exceptions(std::ifstream::failbit);

            file >> n;

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    int distance;
                    file >> distance;
                    adjMatrix.push_back(distance);
                }
            }

        } catch(const std::ios_base::failure& e) {
            std::cout << e.what() << std::endl;
            std::cout << strerror(errno) << std::endl;
            file.clear();
        }

        return Tsp{adjMatrix, n};
    }

    size_t size() const { return n; }

    int& get(size_t x, size_t y) {
        return adjMatrix[y * n + x];
    }

    const int& get(size_t x, size_t y) const {
        return adjMatrix[y * n + x];
    }

    std::vector<int> getAdjMatrix() {
        return adjMatrix;
    }

    int cost(const std::vector<int>& order) const {
        int sum = 0;
        size_t prevCity = order[0];
        for (size_t i = 1; i < order.size(); ++i) {
            sum += get(prevCity, order[i]);
            prevCity = order[i];
        }
        return sum;
    }

    void print() const {
        for(int y = 0; y < n; ++y) {
            for(int x = 0; x < n; ++x) {
                std::cout << get(x, y) << " ";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }
};
