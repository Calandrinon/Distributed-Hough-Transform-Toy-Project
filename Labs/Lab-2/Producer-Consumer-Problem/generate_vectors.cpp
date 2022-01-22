//
// Created by calandrinon on 10/9/21.
//

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <fstream>

int main() {
    std::ofstream out("vectors.txt");
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    auto randomEngine = std::default_random_engine(seed);
    int numberOfElements = 100000;
    out << numberOfElements << "\n";

    for (int i = 0; i < numberOfElements; i++)
        out << 100 << " " << 100 << "\n";

    return 0;
}