//
// Created by calandrinon on 10/2/21.
//

#ifndef BANKACCOUNTS_BANKACCOUNTREPOSITORY_H
#define BANKACCOUNTS_BANKACCOUNTREPOSITORY_H
#include "../Model/BankAccount.h"
#include "../Model/BankDetailsState.h"


class BankAccountRepository {
    private:
        std::vector<BankAccount*> container;
        OperationLog* globalOperationLog;
        BankDetailsState* bankDetailsState;
        int numberOfRecentlyExecutedOperations;
        std::mutex numberOfOperationsMutex, globalRecordsMutex;

    public:
        BankAccountRepository();
        void save(BankAccount* account);
        BankDetailsState *getBankDetailsState() const;
        void setBankDetailsState(BankDetailsState *bankDetailsState);
        void addGlobalOperationRecord(Operation* operation);
        OperationLog* getGlobalOperationLog();
        std::vector<BankAccount*> getContainer() const;
        void setNumberOfRecentlyExecutedOperations(int numberOfRecentlyExecutedOperations);
        int getNumberOfRecentlyExecutedOperations();
};


#endif //BANKACCOUNTS_BANKACCOUNTREPOSITORY_H
