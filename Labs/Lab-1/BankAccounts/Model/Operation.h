//
// Created by calandrinon on 10/2/21.
//

#ifndef BANKACCOUNTS_OPERATION_H
#define BANKACCOUNTS_OPERATION_H
#include <iostream>
#include <ctime>
#include <chrono>


class Operation {
    private:
        int serialNumber;
        std::string transferer, transferee;
        std::time_t transactionTime;
        double amount;

    public:
        Operation(std::string transferer, std::string transferee, double amount);
        int getSerialNumber() const;
        void setSerialNumber(int serialNumber);
        time_t getTransactionTime() const;
        double getAmount();
        void setAmount(double amount);
        std::string toString();
        ~Operation() = default;

    const std::string &getTransferer() const;

    void setTransferer(const std::string &transferer);

    const std::string &getTransferee() const;

    void setTransferee(const std::string &transferee);

    void setTransactionTime(time_t transactionTime);

    bool equals(Operation* otherOperation);
};


#endif //BANKACCOUNTS_OPERATION_H
