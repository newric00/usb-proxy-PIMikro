#include <iostream>
#include <vector>
#include "GCS-parser.h"

void testParser(const std::string& rawCommand) {
    std::vector<uint8_t> data(rawCommand.begin(), rawCommand.end());

    //Parse
    std::string result = parseGCSCommand(data);

    std::cout << "Input: " <<rawCommand << "\nOutput: " << result << "\n" << std::endl;
}

int main() {
    testParser("MVR 1 -2");
    testParser({0x07});
    return 0;
}