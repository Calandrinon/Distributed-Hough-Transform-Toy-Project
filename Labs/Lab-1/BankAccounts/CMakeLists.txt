cmake_minimum_required(VERSION 3.20)
project(BankAccounts)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS -pthread)


add_executable(BankAccounts main.cpp Model/BankAccount.cpp Model/BankAccount.h Model/OperationLog.cpp Model/OperationLog.h Model/Operation.cpp Model/Operation.h Repository/BankAccountRepository.cpp Repository/BankAccountRepository.h Model/Model.h Model/BankDetailsState.cpp Model/BankDetailsState.h Service/BankService.cpp Service/BankService.h UI/BankUI.cpp UI/BankUI.h)
