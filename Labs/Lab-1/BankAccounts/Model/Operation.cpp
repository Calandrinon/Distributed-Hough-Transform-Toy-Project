//
// Created by calandrinon on 10/2/21.
//

#include "Operation.h"
#include <string>

Operation::Operation(std::string transferer, std::string transferee, double amount) {
    this->transferer = transferer;
    this->transferee = transferee;
    this->amount = amount;
    this->transactionTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

int Operation::getSerialNumber() const { return serialNumber;
}

void Operation::setSerialNumber(int serialNumber) {
    this->serialNumber = serialNumber;
}

time_t Operation::getTransactionTime() const {
    return transactionTime;
}

double Operation::getAmount() {
    return this->amount;
}

void Operation::setAmount(double amount) {
    this->amount = amount;
}

const std::string &Operation::getTransferer() const {
    return transferer;
}

void Operation::setTransferer(const std::string &transferer) {
    Operation::transferer = transferer;
}

const std::string &Operation::getTransferee() const {
    return transferee;
}

void Operation::setTransferee(const std::string &transferee) {
    Operation::transferee = transferee;
}

void Operation::setTransactionTime(time_t transactionTime) {
    Operation::transactionTime = transactionTime;
}

std::string Operation::toString() {
    return "Serial number: " + std::to_string(this->serialNumber) + "; Transferrer: " + this->transferer + "; Transferree: " + this->transferee + "; Amount: " + std::to_string(this->amount) + "; Transaction time: " + std::to_string(this->transactionTime);
}

bool Operation::equals(Operation *otherOperation) {
    return
        this->serialNumber    == otherOperation->getSerialNumber()    &&
        this->transferer      == otherOperation->getTransferer()      &&
        this->transferee      == otherOperation->getTransferee()      &&
        this->transactionTime == otherOperation->getTransactionTime() &&
        this->amount          == otherOperation->getAmount();
}
