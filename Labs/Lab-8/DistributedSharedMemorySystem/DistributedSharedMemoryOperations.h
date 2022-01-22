//
// Created by calandrinon on 12/22/21.
//

#ifndef DISTRIBUTEDSHAREDMEMORYSYSTEM_DISTRIBUTEDSHAREDMEMORYOPERATIONS_H
#define DISTRIBUTEDSHAREDMEMORYSYSTEM_DISTRIBUTEDSHAREDMEMORYOPERATIONS_H
#include <map>
#include <set>
#include <mutex>

class DistributedSharedMemoryOperations {
    private:
        std::map<std::string, std::set<int>> subscriberTable;
        int firstVariable, secondVariable, thirdVariable, fourthVariable;
        std::mutex mutex, subscriptionMutex, synchronizationMutex;

    public:
        DistributedSharedMemoryOperations();
        void updateResource(std::string variableName, int value);
        void setVariable(std::string variableName, int value);
        void compareAndExchange(std::string variableName, int valueToBeCompared, int valueToSet);
        void subscribe(std::string variableName);
        void synchronize(std::string variableName, int processRank);
        void notifySubscribers(std::string variableName, std::string message);
        void sendMessageToAllProcesses(std::string message);
        void end();
        int getFirstVariable();
        int getSecondVariable();
        int getThirdVariable();
        int getFourthVariable();
        std::map<std::string, std::set<int>> getSubscriberTable();
};


#endif //DISTRIBUTEDSHAREDMEMORYSYSTEM_DISTRIBUTEDSHAREDMEMORYOPERATIONS_H
