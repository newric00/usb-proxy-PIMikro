#include <iostream>
#include <string>
#include "GCS-parser.h"

void testSingleByteCommands() {
    std::cout << "Testing Single-Byte Commands:\n";

    std::string command1 = "\x04"; // Example: #4 (single-byte)
    auto result1 = parseGCSCommand(command1);
    std::cout << "Input: #4\nParsed Output: " << result1 << "\n\n";

    std::string commandWithAddress = "1 \x04"; // Example: 1 #4
    auto result2 = parseGCSCommand(commandWithAddress);
    std::cout << "Input: 1 #4\nParsed Output: " << result2 << "\n\n";
}

void testMultiCharCommands() {
    std::cout << "Testing Multi-Character Commands:\n";

    std::string command1 = "ERR?";
    auto result1 = parseGCSCommand(command1);
    std::cout << "Input: ERR?\nParsed Output: " << result1 << "\n\n";

    std::string addressedCommand = "1 0 ERR?";
    auto result2 = parseGCSCommand(addressedCommand);
    std::cout << "Input: 1 0 ERR?\nParsed Output: " << result2 << "\n\n";
}

void testInvalidCommands() {
    std::cout << "Testing Invalid Commands:\n";

    std::string invalidCommand = "XYZ";
    auto result = parseGCSCommand(invalidCommand);
    std::cout << "Input: XYZ\nParsed Output: " << result << "\n\n";
}

void testResponseParsing() {
    std::cout << "Testing Response Parsing:\n";

    std::vector<uint8_t> response1 = {'0', '\n'}; // Valid response
    auto parsedResponse = parseGCSResponse(response1);
    std::cout << "Input: 0\nParsed Output: " << parsedResponse.response
              << " | Length: " << parsedResponse.responseLength << "\n\n";

    std::vector<uint8_t> responseWithBackticks = {'`', '0', '\n'};
    auto parsedResponse2 = parseGCSResponse(responseWithBackticks);
    std::cout << "Input: `0\nParsed Output: " << parsedResponse2.response
              << " | Length: " << parsedResponse2.responseLength << "\n\n";
}

int main() {
    testSingleByteCommands();
    testMultiCharCommands();
    testInvalidCommands();
    testResponseParsing();
    return 0;
}