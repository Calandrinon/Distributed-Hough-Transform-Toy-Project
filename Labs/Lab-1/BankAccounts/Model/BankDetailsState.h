//
// Created by calandrinon on 10/2/21.
//

#ifndef BANKACCOUNTS_BANKDETAILSSTATE_H
#define BANKACCOUNTS_BANKDETAILSSTATE_H


#include <mutex>

class BankDetailsState {
    private:
        int transactionSerialNumber, accountSerialNumber;
        std::mutex serialNumberMutex;

    public:
        BankDetailsState();
        ~BankDetailsState();
        int getTransactionSerialNumber() const;
        int getAndIncrementTransactionSerialNumber();
        void setTransactionSerialNumber(int transactionSerialNumber);
        int getAccountSerialNumber() const;
        void setAccountSerialNumber(int accountSerialNumber);
};


#endif //BANKACCOUNTS_BANKDETAILSSTATE_H
