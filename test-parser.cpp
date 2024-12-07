#include <iostream>
#include <vector>
#include "GCS-parser.h"

void testParser(const std::vector<uint8_t>& rawData) {
    // Parse the command
    std::string result = parseGCSCommand(rawData);

    // Print the input and output
    std::cout << "Input: ";
    for (auto byte : rawData) {
        std::cout << "0x" << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << "\nParsed Output: " << result << "\n" << std::endl;
}

void testGetCommandInfo(const std::string& command) {
    auto info = getCommandInfo(command);
    if (info) {
        std::cout << "Command: " << command << "\nLong Description: " << info->longDescription << "\nArguments: ";
        for (const auto& arg : info->argumentNames) {
            std::cout << arg << " ";
        }
        std::cout << "\n" << std::endl;
    } else {
        std::cout << "Command: " << command << "\nResult: Unknown Command\n" << std::endl;
    }
}

int main() {
    // Test single-byte commands
    std::cout << "Testing Single-Byte Commands:" << std::endl;
    testParser({0x04}); // Request Status Register
    testParser({0x07}); // Request Controller Ready Status

    // Test multi-character commands
    std::cout << "Testing Multi-Character Commands:" << std::endl;
    testParser({'*', 'I', 'D', 'N', '?'}); // Get Device Identification
    testParser({'A', 'C', 'C', ' ', '1', ' ', '0', '.', '5'}); // Set Acceleration

    // Test unknown commands
    std::cout << "Testing Unknown Commands:" << std::endl;
    testParser({'X', 'Y', 'Z'}); // Unknown command

    // Test getCommandInfo
    std::cout << "Testing getCommandInfo:" << std::endl;
    testGetCommandInfo("*IDN?");
    testGetCommandInfo("ACC");
    testGetCommandInfo("XYZ"); // Unknown command

    return 0;
}
