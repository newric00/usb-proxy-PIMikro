#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

const std::unordered_map<uint8_t, std::string> controlCommands = {
    {0x07, "Request Controller Ready Status"},
    {0x08, "Query if Macro is Running"}
};

std::string parseGCSCommand(const std::vector<uint8_t>& data) {
    
    //convert raw data to string
    std::string command(data.begin(), data.end());

    if (command.size() == 1) {
        uint8_t byteValue = command[0];
        auto it = controlCommands.find(byteValue);

        if (it != controlCommands.end()) {
            return "Command: #" + std::to_string(byteValue) + "\n";
        }
        return "Unknown Control Command: 0x" + std::to_string(byteValue) + "\n";
    }

    //split command by spaces
    std::stringstream ss(command);
    std::string token;
    std::vector<std::string> tokens;

    while (ss >> token) {
        tokens.push_back(token);
    }

    //MVR MOVE RELATIVE: ARGUMENTS AXIS, PHYSICAL DISTANCE
    if(tokens[0] == "MVR" && tokens.size() == 3) {
        return "Command: " + tokens[0] + " | Axis: " + tokens[1] + " | Phys_distance: " + tokens[2] + " mm" + "\n";
    }
    //UNRECOGNIZED COMMAND > RETURN RAW COMMAND
    return "Raw Command: " + command;
}