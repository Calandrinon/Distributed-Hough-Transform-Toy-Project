//
// Created by calandrinon on 11/29/21.
//

#include <random>
#include <chrono>
#include <fstream>
#include <memory>
#include "Graph.h"

Graph::Graph(int _numberOfNodes): numberOfNodes(_numberOfNodes) {
    for (int index = 0; index < _numberOfNodes; index++)
        this->container.emplace_back();
}

void Graph::addEdge(int node, int otherNode) {
    this->container[node].push_back(otherNode);
}

std::vector<int> Graph::getAdjacentNodes(int node) {
    return this->container[node];
}

int Graph::getNumberOfNodes() {
    return this->numberOfNodes;
}

void Graph::turnIntoCompleteGraph() {
    for (int firstIndex = 0; firstIndex < this->container.size(); firstIndex++) {
        for (int secondIndex = 0; secondIndex < this->container.size(); secondIndex++)
            if (firstIndex != secondIndex)
                this->container[firstIndex].push_back(secondIndex);
    }
}

void Graph::turnIntoRandomGraph() {
    auto randomEngine = std::default_random_engine(std::chrono::steady_clock::now().time_since_epoch().count());

    std::vector<int> hamiltonianPath;
    hamiltonianPath.reserve(this->numberOfNodes);
    for (int index = 0; index < this->numberOfNodes; index++)
        hamiltonianPath.push_back(index);

    std::shuffle(hamiltonianPath.begin(), hamiltonianPath.end(), std::mt19937(std::random_device()()));

    for (int index = 1; index < this->numberOfNodes; index++)
        this->addEdge(hamiltonianPath[index - 1], hamiltonianPath[index]);
    this->addEdge(hamiltonianPath[this->numberOfNodes - 1], hamiltonianPath[0]);

    for (int nodeIndex = 0; nodeIndex < this->numberOfNodes; nodeIndex++) {
        std::shuffle(hamiltonianPath.begin(), hamiltonianPath.end(), std::mt19937(std::random_device()()));

        // Ghouila-Houiri theorem states that a strongly connected directed graph with n vertices is Hamiltonian if
        // every vertex has a full degree greater than or equal to n.
        // We add for each node (nodeIndex) an outgoing edge (if the edge doesn't already exist) and an incoming edge
        // from a random node.

        for (int otherNodeIndex = 0; otherNodeIndex < this->numberOfNodes / 2; otherNodeIndex++) {
            if (!std::count(this->container[nodeIndex].begin(), this->container[nodeIndex].end(), otherNodeIndex)) {
                this->addEdge(nodeIndex, otherNodeIndex);
                int startingNodeOfIncomingEdge = (int)randomEngine() % this->numberOfNodes;
                while (startingNodeOfIncomingEdge == nodeIndex || std::count(this->container[startingNodeOfIncomingEdge].begin(), this->container[startingNodeOfIncomingEdge].end(), nodeIndex))
                    startingNodeOfIncomingEdge = (int)randomEngine() % this->numberOfNodes;
                this->addEdge(startingNodeOfIncomingEdge, nodeIndex);
            }
        }
    }
}

bool Graph::doesEdgeExist(int firstNode, int secondNode) {
    return std::find(this->container[firstNode].begin(), this->container[firstNode].end(), secondNode) != this->container[firstNode].end();
}

void Graph::print() {
    std::cout << "Edges of the graph:\n";

    for (int nodeIndex = 0; nodeIndex < this->container.size(); nodeIndex++) {
        for (int neighbourIndex = 0; neighbourIndex < this->container[nodeIndex].size(); neighbourIndex++)
            std::cout << "(" << nodeIndex << "; " << this->container[nodeIndex][neighbourIndex] << ")\n";
    }
}

std::unique_ptr<Graph> Graph::readFromFile(const std::string& filename) {
    std::ifstream in("../" + filename);
    int numberOfNodes, numberOfEdges, firstNode, secondNode;
    in >> numberOfNodes >> numberOfEdges;

    std::unique_ptr<Graph> graph = std::make_unique<Graph>(numberOfNodes);

    for (int edgeIndex = 0; edgeIndex < numberOfEdges; edgeIndex++) {
        in >> firstNode >> secondNode;
        graph->addEdge(firstNode, secondNode);
    }

    return graph;
}

void Graph::saveToFile(const std::string& filename) {
    std::ofstream out("../" + filename);
    int numberOfEdges = 0;
    std::string graphAsText;

    for (int nodeIndex = 0; nodeIndex < this->numberOfNodes; nodeIndex++) {
        for (int neighbourIndex = 0; neighbourIndex < this->container[nodeIndex].size(); neighbourIndex++) {
            numberOfEdges++;
            graphAsText += std::to_string(nodeIndex ) + " "
                    + std::to_string(this->container[nodeIndex][neighbourIndex]) + "\n";
        }
    }

    out << this->numberOfNodes << " " << numberOfEdges << "\n";
    out << graphAsText;

    out.close();
}
