//
// Created by calandrinon on 10/2/21.
//

#include "OperationLog.h"

OperationLog::OperationLog() {}

void OperationLog::addOperation(Operation *operation) {
    std::unique_lock<std::mutex> lock(this->addOperationMutex);
    this->operations.push_back(operation);
}

std::vector<Operation*> OperationLog::getOperations() {
    this->addOperationMutex.lock();
    std::vector<Operation*> returnedOperations = this->operations;
    this->addOperationMutex.unlock();
    return returnedOperations;
}

OperationLog::~OperationLog() {
    for (Operation* operation: this->operations) {
        delete operation;
    }
}
