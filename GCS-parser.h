#ifndef GCS_PARSER_H
#define GCS_PARSER_H

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

struct GCSCommandSurvey {
    std::string longDescription;
    std::vector<std::string> argumentNames;
};

std::string parseGCSCommand(const std::vector<uint8_t>& data);
std::string handleBulkInResponse(const std::vector<uint8_t>& data, bool isNewCommand);
std::optional<GCSCommandSurvey> getCommandInfo(const std::string& command);
#endif