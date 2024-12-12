#include <iostream>
#include "GCS-parser.h"
#include "command-utils.h"  // Includes printCommand and other utilities

void testPrintCommand(const std::string &command) {
    // Use ParsedCommand to create a structure for testing
    ParsedCommand testCommand;
    testCommand.command = command;              // Raw command for determining color
    testCommand.decoratedCommand = "Command: " + command;  // Decorated command for printing

    printCommand(testCommand);  // Test printing
    std::cout << std::endl; // For spacing
}

int main() {
    std::cout << "Testing GCS Command Parsing and Colored Printing" << std::endl;

    // Example commands to test
    std::string command1 = "ERR?";
    std::string command2 = "MOV";
    std::string command3 = "#8";
    std::string command4 = "INVALID";

    // Test with printCommand,
    testPrintCommand(command1);
    testPrintCommand(command2);
    testPrintCommand(command3);
    testPrintCommand(command4);

    return 0;
}
