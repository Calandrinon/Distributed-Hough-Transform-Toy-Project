//
// Created by calandrinon on 10/2/21.
//

#ifndef BANKACCOUNTS_OPERATIONLOG_H
#define BANKACCOUNTS_OPERATIONLOG_H
#include <vector>
#include "Operation.h"
#include <mutex>


class OperationLog {
    private:
        std::vector<Operation*> operations;
        std::mutex addOperationMutex;

    public:
        OperationLog();
        void addOperation(Operation* operation);
        std::vector<Operation*> getOperations();
        ~OperationLog();
};


#endif //BANKACCOUNTS_OPERATIONLOG_H
