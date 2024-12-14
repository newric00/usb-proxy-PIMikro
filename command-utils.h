#ifndef COMMAND_utils_H
#define COMMAND_utils_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <jsoncpp/json/json.h>
#include "GCS-parser.h"

//for coupling responses to command
extern LastCommandInfo lastCommandInfo;
extern std::mutex lastCommandMutex;

// Terminal escape sequences for colors
extern const std::unordered_map<std::string, std::string> colorMap;

// command color mapping
extern std::unordered_map<std::string, std::string> commandColorMapping;

// Function declarations
void loadCommandGroups(const Json::Value& commandGroups, 
                      std::unordered_map<std::string, std::string>& mapping);
std::string getCommandColor(const std::string& command);
void printCommand(const ParsedCommand& parsedCommand);
#endif