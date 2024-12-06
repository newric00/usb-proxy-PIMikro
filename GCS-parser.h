#ifndef GCS_PARSER_H
#define GCS_PARSER_H

#include <string>
#include <vector>

std::string parseGCSCommand(const std::vector<uint8_t>& data);
std::string handleBulkInResponse(const std::vector<uint8_t>& data, bool isNewCommand);
#endif