#include <iostream>
#include <memory>
#include "Graph.h"
#include "HamiltonianCycleDetector.h"

void sequentialImplementation(Graph* graph) {
    HamiltonianCycleDetector hamiltonianCycleDetector;
    hamiltonianCycleDetector.sequentialHamiltonianCycleDetection(*graph);

    if (hamiltonianCycleDetector.hasSolution()) {
        std::vector<int> hamiltonianCycle = hamiltonianCycleDetector.getHamiltonCycle();
        std::cout << "Sequential solution -> The Hamiltonian cycle is:\n";
        for (int nodeIndex : hamiltonianCycle)
            std::cout << nodeIndex << " ";
        std::cout << "\n";
    } else {
        std::cout << "The graph has no Hamiltonian cycle.\n";
    }
}

void parallelImplementation(Graph* graph) {
    HamiltonianCycleDetector hamiltonianCycleDetector;
    hamiltonianCycleDetector.parallelHamiltonianCycleDetection(*graph);

    if (hamiltonianCycleDetector.hasSolution()) {
        std::vector<int> hamiltonianCycle = hamiltonianCycleDetector.getParallelSolution();
        std::cout << "Parallel solution -> The Hamiltonian cycle is:\n";
        for (int nodeIndex : hamiltonianCycle)
            std::cout << nodeIndex << " ";
        std::cout << "\n";
    } else {
        std::cout << "The graph has no Hamiltonian cycle.\n";
    }
}

void checkSequentialAndParallelImplementationsWithSavedInputGraph() {
    std::unique_ptr<Graph> graph = Graph::readFromFile("example_graph.txt");
    graph->print();

    sequentialImplementation(&(*graph));
    parallelImplementation(&(*graph));
}

void checkSequentialAndParallelImplementationsWithRandomInputGraph(int numberOfNodes) {
    Graph graph(numberOfNodes);
    graph.turnIntoRandomGraph();
    graph.saveToFile("last_generated_graph.txt");

    auto startSequential = std::chrono::steady_clock::now();
    sequentialImplementation(&graph);
    auto endSequential = std::chrono::steady_clock::now();

    auto startParallel = std::chrono::steady_clock::now();
    parallelImplementation(&graph);
    auto endParallel = std::chrono::steady_clock::now();

    std::chrono::duration<double> sequentialExecutionTime = endSequential - startSequential;
    std::chrono::duration<double> parallelExecutionTime = endParallel - startParallel;

    std::cout << "Execution time:\n";
    std::cout << "Sequential implementation -> " << sequentialExecutionTime.count() << " seconds\n";
    std::cout << "Parallel implementation -> " << parallelExecutionTime.count() << " seconds\n";
}

int main() {
    checkSequentialAndParallelImplementationsWithSavedInputGraph();
    //checkSequentialAndParallelImplementationsWithRandomInputGraph(500);
    return 0;
}
