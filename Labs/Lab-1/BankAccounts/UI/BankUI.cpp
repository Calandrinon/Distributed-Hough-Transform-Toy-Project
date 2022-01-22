//
// Created by calandrinon on 10/2/21.
//

#include "BankUI.h"
#include <thread>
#include <random>

BankUI::BankUI(BankService *service) {
    this->service = service;
}

void BankUI::runExample() {
    std::cout << "BankUI started successfully.\n";
    BankAccount* jack = this->service->createAccount("Jack", 1000);
    BankAccount* jill = this->service->createAccount("Jill", 1000);

    for (BankAccount* account: this->service->getAccounts()) {
        std::cout << "Account ID: " << account->getId() << "; Account owner: " << account->getOwner() << ";\n";
    }

    this->service->transferMoney(jack, jill, 100);

    std::cout << "Jack's balance: " << jack->getBalance() << "\n";
    std::cout << "Jill's balance: " << jill->getBalance() << "\n";

    std::cout << "Jack's operations: " << "\n";
    for (Operation* operation: jack->getOperationLog()->getOperations()) {
        std::cout << operation->getAmount() << "$ transferred from " << operation->getTransferer() << " to " << operation->getTransferee() << "\n";
    }

    std::cout << "Jill's operations: " << "\n";
    for (Operation* operation: jill->getOperationLog()->getOperations()) {
        std::cout << operation->getAmount() << "$ transferred from " << operation->getTransferer() << " to " << operation->getTransferee() << "\n";
    }
}

int BankUI::readNumberOfThreads() {
    int numberOfThreads;
    std::cout << "Enter the number of threads: ";
    std::cin >> numberOfThreads;
    return numberOfThreads;
}

void BankUI::runValidThreadConfigurationWithLocks() {
    std::cout << "Starting the transactions...\n";

    std::vector<std::thread> threads;

    int numberOfThreads = 1000;// this->readNumberOfThreads();
    BankAccount* jack = this->service->createAccount("Jack", numberOfThreads);
    BankAccount* jill = this->service->createAccount("Jill", numberOfThreads);

    jack->setBalance(numberOfThreads);
    jill->setBalance(numberOfThreads);

    for (BankAccount* account: this->service->getAccounts()) {
        std::cout << "Account ID: " << account->getId() << "; Account owner: " << account->getOwner() << ";\n";
    }

    auto thread_lambda = [this](BankAccount* transferer, BankAccount* transferee, double sum) {
        this->service->transferMoney(transferer, transferee, sum);
        std::cout << "Transferred " << sum << "$ from " << transferer->getOwner() << " to " << transferee->getOwner() << "\n";
    };

    for (int thread_index = 0; thread_index < numberOfThreads; thread_index++) {
        threads.emplace_back(thread_lambda, jack, jill, 1);
        threads.emplace_back(thread_lambda, jill, jack, 1);
        threads.emplace_back(thread_lambda, jack, jill, 1);
        threads.emplace_back(thread_lambda, jill, jack, 1);
    }

    for (std::thread& thread: threads) {
        thread.join();
    }

    std::cout << "Jack's balance: " << jack->getBalance() << "\n";
    std::cout << "Jill's balance: " << jill->getBalance() << "\n";

    std::cout << "Joined all threads.\n";
}

void BankUI::run() {
    /**
     * Validity checks:
     *      - the total sum of the balances of each account before the transfers should be equal to the sum after the
     *        transfers
     *      - periodic check of the transfer log once in 100 transfers (to see if the transfers are saved in the logs of
     *        both the bank accounts)
     *      - periodic check of the transfer log once in 100 transfers (to see if the balances of the accounts are
     *        consistent according to the logs)
     * **/
    std::cout << "Starting the transactions...\n";
    std::vector<std::thread> threads;
    int numberOfThreads = 100;

    this->service->createRandomAccounts(100);
    double totalBalanceBeforeOperations = this->service->getTotalAccountsBalance();

    for (BankAccount* account: this->service->getAccounts()) {
        std::cout << "Account ID: " << account->getId() << "; Account owner: " << account->getOwner() << "; Balance: " << account->getBalance() << ";\n";
    }
    std::cout << "The total balance before the operations: " << totalBalanceBeforeOperations << "\n";

    this->service->createRandomMultithreadedOperations(numberOfThreads);
    double totalBalanceAfterOperations = this->service->getTotalAccountsBalance();

    std::cout << "The total balance after the operations: " << totalBalanceAfterOperations << "\n";

    if (totalBalanceBeforeOperations != totalBalanceAfterOperations) {
        std::cout << "Failure: The total balance before the operations differs from the current one!\n";
    } else {
        std::cout << "Success: The total balance before the operations is identical to the current one.\n";
    }
}

