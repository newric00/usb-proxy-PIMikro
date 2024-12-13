#include "command-utils.h"
#include <iostream>

// Terminal escape sequences for colors
const std::unordered_map<std::string, std::string> colorMap = {
    {"purple", "\033[35m"},
    {"blue", "\033[34m"},
    {"grey", "\033[90m"},
    {"green", "\033[32m"},
    {"gold", "\033[33m"},
    {"reset", "\033[0m"},
    {"bold", "\033[1m"}
};

//define command color mapping
std::unordered_map<std::string, std::string> commandColorMapping;

void loadCommandGroups(const Json::Value& commandGroups, 
                       std::unordered_map<std::string, std::string>& commandColorMapping) {
    for (const auto& groupName : commandGroups["groups"].getMemberNames()) {
        const auto& group = commandGroups["groups"][groupName];
        std::string groupColor = group["color"].asString();

        if (colorMap.find(groupColor) == colorMap.end()) {
            printf("Warning: Color '%s' not found in colorMap. Defaulting to 'reset'.\n", groupColor.c_str());
            groupColor = "reset";
        }

        for (const auto& command : group["commands"]) {
            commandColorMapping[command.asString()] = colorMap.at(groupColor);
            std::cout << "Command: " << command.asString() << ", Color: " <<groupColor << std::endl;
        }
    }
}

std::string getCommandColor(const std::string& command) {
    //static std::unordered_map<std::string, std::string> commandColorMapping; 
    auto it = commandColorMapping.find(command);
    return it != commandColorMapping.end() ? it->second : colorMap.at("reset");
}

void printCommand(const ParsedCommand& parsedCommand) {
    std::string color = getCommandColor(parsedCommand.commandCode);
    printf("%s%s%s", color.c_str(), parsedCommand.decoratedCommand.c_str(), colorMap.at("reset").c_str());
}
