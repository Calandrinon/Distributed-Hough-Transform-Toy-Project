//
// Created by calandrinon on 10/2/21.
//

#include "BankDetailsState.h"

BankDetailsState::BankDetailsState() {
    this->transactionSerialNumber = 0;
    this->accountSerialNumber = 0;
}

BankDetailsState::~BankDetailsState() {

}

int BankDetailsState::getTransactionSerialNumber() const {
    return transactionSerialNumber;
}

void BankDetailsState::setTransactionSerialNumber(int transactionSerialNumber) {
    BankDetailsState::transactionSerialNumber = transactionSerialNumber;
}

int BankDetailsState::getAndIncrementTransactionSerialNumber() {
    std::unique_lock serialNumberLock(this->serialNumberMutex);
    this->transactionSerialNumber++;
    return this->transactionSerialNumber;
}

int BankDetailsState::getAccountSerialNumber() const {
    return this->accountSerialNumber;
}

void BankDetailsState::setAccountSerialNumber(int accountSerialNumber) {
    this->accountSerialNumber = accountSerialNumber;
}
