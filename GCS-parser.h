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

struct ParsedResponse {
    std::string response;
    size_t responseLength;
};

std::string processCommand(
    const std::string& command,
    const std::string& controllerAddress,
    const std::string& hostAddress,
    const std::optional<GCSCommandSurvey>& commandInfo,
    const std::vector<std::string>& tokens,
    size_t tokenIndex
);

std::string parseGCSCommand(const std::string& command);
ParsedResponse parseGCSResponse(const std::vector<uint8_t>& data);
std::string cleanResponse(const std::string& response);
std::optional<ParsedResponse> handleBulkInResponse(const std::vector<uint8_t>& data);
std::optional<GCSCommandSurvey> getCommandInfo(const std::string& command);
#endif