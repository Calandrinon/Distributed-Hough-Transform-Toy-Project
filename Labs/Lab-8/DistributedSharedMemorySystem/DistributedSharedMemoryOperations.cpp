//
// Created by calandrinon on 12/22/21.
//

#include "DistributedSharedMemoryOperations.h"
#include "Message.h"
#include "mpi.h"
#define MPI_MESSAGE_LENGTH 0
#define MPI_MESSAGE 1

DistributedSharedMemoryOperations::DistributedSharedMemoryOperations() {
    this->firstVariable = 1; this->secondVariable = 2; this->thirdVariable = 3;this->fourthVariable = 4;
    this->subscriberTable["firstVariable"] = std::set<int>();
    this->subscriberTable["secondVariable"] = std::set<int>();
    this->subscriberTable["thirdVariable"] = std::set<int>();
    this->subscriberTable["fourthVariable"] = std::set<int>();
}

void DistributedSharedMemoryOperations::updateResource(std::string variableName, int value) {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->setVariable(variableName, value);
    std::string message = Message::serializeUpdateMessage(variableName, value);
    this->notifySubscribers(variableName, message);
}

void DistributedSharedMemoryOperations::setVariable(std::string variableName, int value) {
    this->firstVariable = (variableName == "firstVariable" ? value : firstVariable);
    this->secondVariable = (variableName == "secondVariable" ? value : secondVariable);
    this->thirdVariable = (variableName == "thirdVariable" ? value : thirdVariable);
    this->fourthVariable = (variableName == "fourthVariable" ? value : fourthVariable);
}

void
DistributedSharedMemoryOperations::compareAndExchange(std::string variableName, int valueToBeCompared, int valueToSet) {
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    if (subscriberTable[variableName].find(processId) != subscriberTable[variableName].end())
        this->notifySubscribers(variableName, Message::serializeErrorMessage(variableName, processId));
    if (this->firstVariable == valueToBeCompared && variableName == "firstVariable")
        updateResource("firstVariable", valueToSet);
    if (this->secondVariable == valueToBeCompared && variableName == "secondVariable")
        updateResource("secondVariable", valueToSet);
    if (this->thirdVariable == valueToBeCompared && variableName == "thirdVariable")
        updateResource("thirdVariable", valueToSet);
    if (this->fourthVariable == valueToBeCompared && variableName == "fourthVariable")
        updateResource("fourthVariable", valueToSet);
}

void DistributedSharedMemoryOperations::subscribe(std::string variableName) {
    std::unique_lock<std::mutex> lock(subscriptionMutex);
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    subscriberTable[variableName].insert(processId);
    std::string message = Message::serializeSubscriptionMessage(variableName, processId);
    sendMessageToAllProcesses(message);
}

void DistributedSharedMemoryOperations::synchronize(std::string variableName, int processId) {
    std::unique_lock<std::mutex> lock(synchronizationMutex);
    this->subscriberTable[variableName].insert(processId);
}

void DistributedSharedMemoryOperations::notifySubscribers(std::string variableName, std::string message) {
    int numberOfProcesses, selfId;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &selfId);
    for (int processId = 0; processId < numberOfProcesses; processId++) {
        if (processId != selfId || this->subscriberTable[variableName].find(processId) != this->subscriberTable[variableName].end())
            continue;

        const char* messageAsCharArray = message.c_str();
        int messageLength = (int)message.size();
        MPI_Send(&messageLength, 1, MPI_INT, processId, MPI_MESSAGE_LENGTH, MPI_COMM_WORLD);
        MPI_Send(messageAsCharArray, messageLength + 1, MPI_CHAR, processId, MPI_MESSAGE, MPI_COMM_WORLD);
    }
}

void DistributedSharedMemoryOperations::sendMessageToAllProcesses(std::string message) {
    int numberOfProcesses, selfId;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &selfId);
    for (int processId = 0; processId < numberOfProcesses; processId++) {
        std::vector<std::string> tokenList = Message::getMessageTokens(message);
        if (processId != selfId && tokenList[0] != "END")
            continue;

        const char* messageAsCharArray = message.c_str();
        int messageLength = (int)message.size();
        MPI_Send(&messageLength, 1, MPI_INT, processId, MPI_MESSAGE_LENGTH, MPI_COMM_WORLD);
        MPI_Send(messageAsCharArray, messageLength + 1, MPI_CHAR, processId, MPI_MESSAGE, MPI_COMM_WORLD);
    }
}

void DistributedSharedMemoryOperations::end() {
    sendMessageToAllProcesses(Message::serializeEndMessage());
}

int DistributedSharedMemoryOperations::getFirstVariable() {
    return this->firstVariable;
}

int DistributedSharedMemoryOperations::getSecondVariable() {
    return this->secondVariable;
}

int DistributedSharedMemoryOperations::getThirdVariable() {
    return this->thirdVariable;
}

int DistributedSharedMemoryOperations::getFourthVariable() {
    return this->fourthVariable;
}

std::map<std::string, std::set<int>> DistributedSharedMemoryOperations::getSubscriberTable() {
    return this->subscriberTable;
}
