#ifndef GCS_PARSER_H
#define GCS_PARSER_H

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <algorithm>

struct GCSCommandSurvey {
    std::string longDescription;
    std::vector<std::string> argumentNames;
};

struct ParsedCommand {
    std::string commandCode; //command byte code or mnemonic
    std::string decoratedCommand; //response with parsed arguments and long description
    std::string controllerAddress;
    std::string hostAddress; //should always = "0"
};

struct ParsedResponse {
    std::string response;
    size_t responseLength;
};

struct LastCommandInfo {
    std::string commandCode; //command byte code or mnemonic
    std::string controllerAddress;
    std::string hostAddress; //should always = "0"
};

std::string processCommand(
    const std::string& command,
    const std::string& controllerAddress,
    const std::string& hostAddress,
    const std::optional<GCSCommandSurvey>& commandInfo,
    const std::vector<std::string>& tokens,
    size_t tokenIndex
);
ParsedCommand parseGCSCommand(const std::string& command);
ParsedResponse parseGCSResponse(const std::vector<uint8_t>& data, LastCommandInfo& lastCommand);
std::string cleanResponse(const std::string& response);
std::optional<ParsedResponse> handleBulkInResponse(const std::vector<uint8_t>& data, LastCommandInfo& lastCommand);
std::optional<GCSCommandSurvey> getCommandInfo(const std::string& command);
#endif