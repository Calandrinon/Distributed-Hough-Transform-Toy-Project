//
// Created by calandrinon on 10/2/21.
//

#include <map>
#include "BankService.h"

BankService::BankService(BankAccountRepository *repository) {
    this->repository = repository;
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    this->randomEngine = std::default_random_engine(seed);
}

BankAccount* BankService::createAccount(std::string ownerName, double balance) {
    BankAccount* newAccount = new BankAccount(ownerName, balance);
    int accountSerialNumber = this->repository->getBankDetailsState()->getAccountSerialNumber() + 1;
    this->repository->getBankDetailsState()->setAccountSerialNumber(accountSerialNumber);
    newAccount->setId(accountSerialNumber);
    this->repository->save(newAccount);

    return newAccount;
}

void BankService::transferMoney(BankAccount *transferer, BankAccount *transferee, double amount) {
    std::unique_lock<std::mutex> transferLock(this->operationMutex);
    if (transferer->getBalance() < amount)
        return;

    OperationLog* transfererOperationLog = transferer->getOperationLog();
    OperationLog* transfereeOperationLog = transferee->getOperationLog();
    auto* transferOperation = new Operation(transferer->getOwner(), transferee->getOwner(), amount);
    transferOperation->setSerialNumber(this->repository->getBankDetailsState()->getAndIncrementTransactionSerialNumber());
    this->repository->addGlobalOperationRecord(transferOperation);
    this->repository->setNumberOfRecentlyExecutedOperations(this->repository->getNumberOfRecentlyExecutedOperations()+1);
    transfererOperationLog->addOperation(transferOperation);
    transfereeOperationLog->addOperation(transferOperation);

    transferer->setBalance(transferer->getBalance() - amount);
    transferee->setBalance(transferee->getBalance() + amount);
}

std::vector<BankAccount*> BankService::getAccounts() {
    return this->repository->getContainer();
}

std::string BankService::generateRandomString(int length) {
    std::string randomString;
    static const char alphabet[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    randomString.reserve(length);

    for (int i = 0; i < length; ++i)
        randomString += alphabet[this->randomEngine() % (sizeof(alphabet) - 1)];

    return randomString;
}

void BankService::createRandomAccounts(int quantity) {
    for (int accountIndex = 0; accountIndex < quantity; accountIndex++) {
        this->createAccount(generateRandomString(5), this->randomEngine() % 10000 + 1000);
    }
}

double BankService::getTotalAccountsBalance() {
    double totalBalance = 0;
    std::vector<BankAccount*> accounts = this->getAccounts();
    for (BankAccount* account: accounts) {
        totalBalance += account->getBalance();
    }

    return totalBalance;
}

int BankService::generateRandomOperations() {
    for (int operationIndex = 0; operationIndex < this->numberOfOperations; operationIndex++) {
        BankAccount* transferrer = this->pickRandomAccount();
        BankAccount* transferree = this->pickRandomAccount();
        double sum = this->randomEngine() % 100 + 1;
        this->transferMoney(transferrer, transferree, sum);
    }

    OperationValidity transferLogsValidity = this->checkTransferLogsValidity(this->numberOfOperations);
    assert(transferLogsValidity == VALID);
    std::cout << (transferLogsValidity == VALID ? "Valid transfer logs" : "Invalid transfer logs") << "\n";

    return this->numberOfOperations;
}

void BankService::createRandomMultithreadedOperations(int numberOfThreads) {
    for (int thread_index = 0; thread_index < numberOfThreads; thread_index++)
        threads.emplace_back([this]() {
            this->generateRandomOperations();
        });

    for (std::thread& thread: threads)
        thread.join();
}

BankAccount* BankService::pickRandomAccount() {
    std::vector<BankAccount*> accounts = this->repository->getContainer();
    int randomIndex = this->randomEngine() % accounts.size();

    return accounts[randomIndex];
}

bool BankService::compareOperationLogs(Operation* operation, OperationLog *firstOperationLog, OperationLog *secondOperationLog, int lagSize) {
    std::vector<Operation*> transferrerOperationLog = firstOperationLog->getOperations();
    std::vector<Operation*> transferreeOperationLog = secondOperationLog->getOperations();

    auto operationMatchLambda = [operation](Operation* currentOperation) {
        return operation->equals(currentOperation);
    };

    std::vector<Operation*>::iterator firstValidityCheckIterator = std::find_if(transferrerOperationLog.begin(), transferrerOperationLog.end(), operationMatchLambda);
    std::vector<Operation*>::iterator secondValidityCheckIterator = std::find_if(transferreeOperationLog.begin(), transferreeOperationLog.end(), operationMatchLambda);
    if (firstValidityCheckIterator == transferrerOperationLog.end() || secondValidityCheckIterator == transferreeOperationLog.end())
        return false;
    return true;
}

OperationValidity BankService::checkTransferLogsValidity(int lagSize) {
    /**
     * lagSize: the last lagSize records are checked to see if each record is saved in the logs of both the bank
     *          accounts which participated in the transfer.
     * **/

    std::vector<BankAccount*> accounts = this->repository->getContainer();
    std::map<std::string, BankAccount*> account_owners;
    for (BankAccount* account: accounts)
        account_owners[account->getOwner()] = account;

    std::vector<Operation*> operationsVector = this->repository->getGlobalOperationLog()->getOperations();
    auto vector_iterator = operationsVector.rbegin();

    for (; vector_iterator != operationsVector.rbegin() + lagSize; vector_iterator++) {
        Operation* operation = *vector_iterator;
        std::string transferrerName = operation->getTransferer();
        std::string transferreeName = operation->getTransferee();
        if (!this->compareOperationLogs(operation, account_owners[transferrerName]->getOperationLog(), account_owners[transferreeName]->getOperationLog(), lagSize))
            return INVALID;
    }

    return VALID;
}

OperationValidity BankService::checkBalanceValidityAfterTransfers(BankAccount* account) {
    std::vector<Operation*> operations = account->getOperationLog()->getOperations();
    double balanceBeforeOperations = account->getInitialBalance();
    double balanceAfterOperations = account->getBalance();
    std::cout << "Balance before operations: " << balanceBeforeOperations << "\n";
    auto operationsIterator = operations.begin();

    for (; operationsIterator < operations.end(); operationsIterator++) {
        if ((*operationsIterator)->getTransferer() == account->getOwner()) {
            balanceBeforeOperations -= (*operationsIterator)->getAmount();
            std::cout << "Balance decreased with " << (*operationsIterator)->getAmount() << " to " << balanceBeforeOperations << "\n";
        } else {
            balanceBeforeOperations += (*operationsIterator)->getAmount();
            std::cout << "Balance increased with " << (*operationsIterator)->getAmount() << " to " << balanceBeforeOperations << "\n";
        }
    }

    std::cout << "Balance before ops: " << balanceBeforeOperations << "; Account balance: " << balanceAfterOperations << "\n";
    return (balanceBeforeOperations == account->getBalance() ? VALID : INVALID);
}

BankService::~BankService() {

}
