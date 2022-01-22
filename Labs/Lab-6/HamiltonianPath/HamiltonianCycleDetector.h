//
// Created by calandrinon on 11/29/21.
//

#ifndef HAMILTONIANPATH_HAMILTONIANCYCLEDETECTOR_H
#define HAMILTONIANPATH_HAMILTONIANCYCLEDETECTOR_H


#include <vector>
#include "Graph.h"
#include <mutex>
#include <atomic>

class HamiltonianCycleDetector {
    private:
        std::vector<int> possibleHamiltonianCycle, solution;
        std::vector<std::vector<int>> solutionsForEachThread;
        bool solutionFound;
        std::atomic<int> threadCounter;
        std::mutex mutex;

    public:
        HamiltonianCycleDetector();
        void tryFromNode(Graph graph, int currentNode);
        void parallelTryFromNode(Graph graph, int currentNode, std::vector<int>* possibleSolution);
        bool sequentialHamiltonianCycleDetection(Graph graph);
        void parallelHamiltonianCycleDetection(Graph graph);
        std::vector<int> getHamiltonCycle();
        std::vector<int> getParallelSolution();
        bool hasSolution() const;
};


#endif //HAMILTONIANPATH_HAMILTONIANCYCLEDETECTOR_H
