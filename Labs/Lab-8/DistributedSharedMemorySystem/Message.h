//
// Created by calandrinon on 12/22/21.
//

#ifndef DISTRIBUTEDSHAREDMEMORYSYSTEM_MESSAGE_H
#define DISTRIBUTEDSHAREDMEMORYSYSTEM_MESSAGE_H
#include <string>
#include <vector>

class Message {
    public:
        static std::string serializeUpdateMessage(std::string variableName, int value);
        static std::string serializeEndMessage();
        static std::string serializeErrorMessage(std::string variableName, int processId);
        static std::string serializeSubscriptionMessage(std::string variableName, int processId);
        static std::vector<std::string> getMessageTokens(std::string message);
};


#endif //DISTRIBUTEDSHAREDMEMORYSYSTEM_MESSAGE_H
