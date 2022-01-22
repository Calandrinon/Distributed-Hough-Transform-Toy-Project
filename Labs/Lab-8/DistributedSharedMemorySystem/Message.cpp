//
// Created by calandrinon on 12/22/21.
//

#include <sstream>
#include "Message.h"

std::string Message::serializeUpdateMessage(std::string variableName, int value) {
    return "UPDATE," + variableName + "," + std::to_string(value);
}

std::string Message::serializeEndMessage() {
    return "END";
}

std::string Message::serializeErrorMessage(std::string variableName, int processId) {
    return "ERROR," + variableName + "," + std::to_string(processId);
}

std::string Message::serializeSubscriptionMessage(std::string variableName, int processId) {
    return "SUBSCRIPTION," + variableName + "," + std::to_string(processId);
}

std::vector<std::string> Message::getMessageTokens(std::string message) {
    std::stringstream test(message);
    std::string segment;
    std::vector<std::string> segmentList;

    while(std::getline(test, segment, ',')) {
        segmentList.push_back(segment);
    }

    return segmentList;
}
