#include <iostream>
#include <vector>
#include <thread>
#include "mpi.h"
#include "DistributedSharedMemoryOperations.h"
#include "Message.h"

void printTable(DistributedSharedMemoryOperations* dsmOperations) {
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    printf("Process %d has: firstVariable=%d, secondVariable=%d, thirdVariable=%d, fourthVariable=%d\n", processId,
           dsmOperations->getFirstVariable(), dsmOperations->getSecondVariable(), dsmOperations->getThirdVariable(),
           dsmOperations->getFourthVariable());

    std::map<std::string, std::set<int>>::iterator it;
    std::map<std::string, std::set<int>> subscriberTable = dsmOperations->getSubscriberTable();

    for (it = subscriberTable.begin(); it != subscriberTable.end(); it++) {
        std::string subscribersString;

        for (auto const& subscriber: it->second) {
            subscribersString += std::to_string(subscriber);
        }

        printf("Subscribers of variable %s: %s\n", it->first.c_str(), subscribersString.c_str());
    }
}

bool runListenerMessageHandler(const std::string& message, DistributedSharedMemoryOperations* dsmOperations) {
    std::vector<std::string> messageTokens = Message::getMessageTokens(message);
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    if (messageTokens[0] == "ERROR") {
        printf("Process %d: The update operation from process %s failed while trying to update %s\n",
               processId, messageTokens[1].c_str(), messageTokens[2].c_str());
    } else if (messageTokens[0] == "SUBSCRIPTION") {
        printf("Process %d: Subscription for %s received from %s\n", processId, messageTokens[1].c_str(), messageTokens[2].c_str());
        dsmOperations->synchronize(messageTokens[1], std::stoi(messageTokens[2]));
    } else if (messageTokens[0] == "UPDATE") {
        printf("Process %d: Update message for %s received from %s\n", processId, messageTokens[1].c_str(), messageTokens[2].c_str());
        dsmOperations->updateResource(messageTokens[1], std::stoi(messageTokens[2]));
    } else if (messageTokens[0] == "END") {
        return true;
    }
    return false;
}

void runListenerThread(DistributedSharedMemoryOperations* dsmOperations) {
    MPI_Status status[2];
    while (true) {
        int messageLength;
        MPI_Recv(&messageLength, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status[0]);
        char* messageAsCharArray = (char*) malloc(messageLength);
        MPI_Recv( messageAsCharArray, messageLength + 1, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status[1]);
        std::string message(messageAsCharArray);
        if(runListenerMessageHandler(message, dsmOperations))
            break;
        printTable(dsmOperations);
    }
}


int main(int argc, char* argv[]) {
    int processId;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    DistributedSharedMemoryOperations dsmOperations;
    switch (processId) {
        case 0: {
            std::thread thread(runListenerThread, &dsmOperations);

            dsmOperations.subscribe("firstVariable");
            dsmOperations.subscribe("secondVariable");
            dsmOperations.subscribe("thirdVariable");
            dsmOperations.compareAndExchange("firstVariable", 1, 11);
            dsmOperations.compareAndExchange("secondVariable", 2, 22);
            dsmOperations.end();

            thread.join();
            break;
        }
        case 1: {
            std::thread thread(runListenerThread, &dsmOperations);
            dsmOperations.subscribe("firstVariable");
            thread.join();
            break;
        }
        case 2: {
            std::thread thread(runListenerThread, &dsmOperations);
            dsmOperations.subscribe("secondVariable");
            thread.join();
            break;
        }
        case 3: {
            std::thread thread(runListenerThread, &dsmOperations);
            dsmOperations.subscribe("thirdVariable");
            dsmOperations.compareAndExchange("firstVariable", 1, 100);
            dsmOperations.end();
            thread.join();
            break;
        }
    }

    MPI_Finalize();

    return 0;
}
