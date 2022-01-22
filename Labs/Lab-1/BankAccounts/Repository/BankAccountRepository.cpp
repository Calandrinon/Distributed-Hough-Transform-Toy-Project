//
// Created by calandrinon on 10/2/21.
//

#include "BankAccountRepository.h"


BankAccountRepository::BankAccountRepository() {
    this->bankDetailsState = new BankDetailsState();
    this->globalOperationLog = new OperationLog();
    this->numberOfRecentlyExecutedOperations = 0;
}

void BankAccountRepository::save(BankAccount *account) {
    this->container.push_back(account);
}

void BankAccountRepository::addGlobalOperationRecord(Operation* operation) {
    this->globalRecordsMutex.lock();
    this->globalOperationLog->addOperation(operation);
    this->globalRecordsMutex.unlock();
}

BankDetailsState *BankAccountRepository::getBankDetailsState() const {
    return bankDetailsState;
}

void BankAccountRepository::setBankDetailsState(BankDetailsState* bankDetailsState) {
    this->bankDetailsState = bankDetailsState;
}

OperationLog *BankAccountRepository::getGlobalOperationLog() {
    return this->globalOperationLog;
}

void BankAccountRepository::setNumberOfRecentlyExecutedOperations(int numberOfRecentlyExecutedOperations) {
    this->numberOfOperationsMutex.lock();
    this->numberOfRecentlyExecutedOperations = numberOfRecentlyExecutedOperations;
    this->numberOfOperationsMutex.unlock();
}

int BankAccountRepository::getNumberOfRecentlyExecutedOperations() {
    return this->numberOfRecentlyExecutedOperations;
}

std::vector<BankAccount*> BankAccountRepository::getContainer() const {
    return this->container;
}
