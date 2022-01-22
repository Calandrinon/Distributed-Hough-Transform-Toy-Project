//
// Created by calandrinon on 11/29/21.
//

#include <vector>
#include <thread>
#include <mutex>
#include "HamiltonianCycleDetector.h"
#include "Graph.h"

HamiltonianCycleDetector::HamiltonianCycleDetector() {
    this->solutionFound = false;
    this->threadCounter.store(0);
}

void HamiltonianCycleDetector::tryFromNode(Graph graph, int currentNode) {
    if (possibleHamiltonianCycle.size() == graph.getNumberOfNodes()) {
        int firstNodeInPath = possibleHamiltonianCycle[0];
        int lastNodeInPath = possibleHamiltonianCycle[possibleHamiltonianCycle.size() - 1];

        if (graph.doesEdgeExist(lastNodeInPath, firstNodeInPath)) {
            this->solutionFound = true;
            possibleHamiltonianCycle.push_back(firstNodeInPath);
        }
        return;
    } else if (possibleHamiltonianCycle.empty()) {
        possibleHamiltonianCycle.push_back(currentNode);
    }

    for (int nodeIndex = 0; nodeIndex < graph.getNumberOfNodes() && !solutionFound; nodeIndex++) {
        if (!std::count(possibleHamiltonianCycle.begin(), possibleHamiltonianCycle.end(), nodeIndex) && graph.doesEdgeExist(currentNode, nodeIndex)) {
            possibleHamiltonianCycle.push_back(nodeIndex);
            tryFromNode(graph, nodeIndex);
            if (solutionFound)
                return;
            possibleHamiltonianCycle.pop_back();
        }
    }
}

bool HamiltonianCycleDetector::sequentialHamiltonianCycleDetection(Graph graph) {
    for (int nodeIndex = 0; nodeIndex < graph.getNumberOfNodes(); nodeIndex++) {
        this->possibleHamiltonianCycle.clear();
        this->tryFromNode(graph, nodeIndex);
        if (this->solutionFound)
            return true;
    }

    return false;
}

void HamiltonianCycleDetector::parallelTryFromNode(Graph graph, int currentNode, std::vector<int>* possibleSolution) {
    if (possibleSolution->size() == graph.getNumberOfNodes()) {
        int firstNodeInPath = (*possibleSolution)[0];
        int lastNodeInPath = (*possibleSolution)[possibleSolution->size() - 1];

        if (graph.doesEdgeExist(lastNodeInPath, firstNodeInPath)) {
            mutex.lock();
            if (!this->solutionFound) {
                this->solutionFound = true;
                possibleSolution->push_back(firstNodeInPath);
                this->solution = *possibleSolution;
            }
            mutex.unlock();
        }
        return;
    }

    for (int nodeIndex = 0; nodeIndex < graph.getNumberOfNodes() && !solutionFound; nodeIndex++) {
        if (!std::count(possibleSolution->begin(), possibleSolution->end(), nodeIndex) && graph.doesEdgeExist(currentNode, nodeIndex)) {
            possibleSolution->push_back(nodeIndex);
            parallelTryFromNode(graph, nodeIndex, possibleSolution);
            if (solutionFound)
                return;
            possibleSolution->pop_back();
        }
    }
}

void HamiltonianCycleDetector::parallelHamiltonianCycleDetection(Graph graph) {
    std::vector<std::thread> threads;
    int numberOfNodes = graph.getNumberOfNodes();
    int nodesPerThread = (int)(!numberOfNodes / std::thread::hardware_concurrency()
            ? 1
            : numberOfNodes / std::thread::hardware_concurrency());

    for (int threadIndex = 0; threadIndex < std::thread::hardware_concurrency(); threadIndex++)
        this->solutionsForEachThread.emplace_back();

    for (int nodeIndex = 0; nodeIndex < numberOfNodes; nodeIndex += nodesPerThread) {
        threads.emplace_back([=]() mutable {
            int threadIndex = this->threadCounter.fetch_add(1);
            for (int chunkNode = nodeIndex; chunkNode < std::min(nodeIndex + nodesPerThread, numberOfNodes); chunkNode++) {
                this->solutionsForEachThread[threadIndex].clear();
                this->solutionsForEachThread[threadIndex].push_back(chunkNode);
                this->parallelTryFromNode(graph, chunkNode, &(this->solutionsForEachThread[threadIndex]));
            }
        });
    }

    for (int threadIndex = 0; threadIndex < threads.size(); threadIndex++)
        threads[threadIndex].join();
}

std::vector<int> HamiltonianCycleDetector::getHamiltonCycle() {
    return this->possibleHamiltonianCycle;
}

bool HamiltonianCycleDetector::hasSolution() const {
    return this->solutionFound;
}

std::vector<int> HamiltonianCycleDetector::getParallelSolution() {
    return this->solution;
}
