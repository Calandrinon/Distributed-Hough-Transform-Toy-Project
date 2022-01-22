//
// Created by calandrinon on 10/2/21.
//

#include "BankAccount.h"

BankAccount::BankAccount(std::string owner) {
    this->owner = owner;
    this->balance = 0;
    this->operationLog = new OperationLog();
    this->creationDate = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

BankAccount::BankAccount(std::string owner, double balance): BankAccount(owner) {
    this->balance = balance;
    this->initialBalance = balance;
    std::cout << "Balance is: " << this->balance << "; Initial balance: " << this->initialBalance << "\n";
}

BankAccount::~BankAccount() {

}

double BankAccount::getBalance() {
    return this->balance;
}

void BankAccount::setBalance(double balance) {
    this->balance = balance;
}

OperationLog *BankAccount::getOperationLog() {
    return operationLog;
}

void BankAccount::setOperationLog(OperationLog *operationLog) {
    BankAccount::operationLog = operationLog;
}

const std::string &BankAccount::getOwner() const {
    return owner;
}

void BankAccount::setOwner(const std::string &owner) {
    BankAccount::owner = owner;
}

time_t BankAccount::getCreationDate() const {
    return creationDate;
}

void BankAccount::setCreationDate(time_t creationDate) {
    BankAccount::creationDate = creationDate;
}

int BankAccount::getId() {
    return this->id;
}

double BankAccount::getInitialBalance() const {
    return initialBalance;
}

void BankAccount::setId(int id) {
    this->id = id;
}
