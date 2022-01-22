//
// Created by calandrinon on 10/2/21.
//

#ifndef BANKACCOUNTS_BANKSERVICE_H
#define BANKACCOUNTS_BANKSERVICE_H

#include "../Repository/BankAccountRepository.h"
#include <thread>
#include <random>
#include <cassert>

const int numberOfTransferOperations = 100;

enum OperationValidity {
    INVALID,
    VALID
};

class BankService {
    private:
        BankAccountRepository* repository;
        std::mutex operationMutex, balanceCheckMutex;
        std::vector<std::thread> threads;
        std::default_random_engine randomEngine;
        int numberOfOperations = numberOfTransferOperations;

    public:
        BankService(BankAccountRepository* repository);
        BankAccount* createAccount(std::string ownerName, double balance);
        void transferMoney(BankAccount* transferer, BankAccount* transferee, double amount);
        std::vector<BankAccount*> getAccounts();
        void createRandomAccounts(int quantity);
        void createRandomMultithreadedOperations(int numberOfThreads);
        BankAccount* pickRandomAccount();
        double getTotalAccountsBalance();
        std::string generateRandomString(int length);
        int generateRandomOperations();
        OperationValidity checkTransferLogsValidity(int lagSize);
        OperationValidity checkBalanceValidityAfterTransfers(BankAccount* account);
        bool compareOperationLogs(Operation* operation, OperationLog* firstOperationLog, OperationLog* secondOperationLog, int lagSize);
        ~BankService();
};


#endif //BANKACCOUNTS_BANKSERVICE_H
