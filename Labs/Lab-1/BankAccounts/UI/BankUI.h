//
// Created by calandrinon on 10/2/21.
//

#ifndef BANKACCOUNTS_BANKUI_H
#define BANKACCOUNTS_BANKUI_H

#include "../Service/BankService.h"


class BankUI {
    private:
        BankService* service;

    public:
        BankUI(BankService* service);
        void runExample();
        int readNumberOfThreads();
        void runValidThreadConfigurationWithLocks();
        void run();
};


#endif //BANKACCOUNTS_BANKUI_H
