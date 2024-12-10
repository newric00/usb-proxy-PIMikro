#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>

//static variables for bulk-in message aggregation
static std::string responseBuffer;
//static int bulkInByteCount = 0;

//command survey
struct GCSCommandSurvey {
    std::string longDescription;
    std::vector<std::string> argumentNames;
};

//single-byte commands
const std::unordered_map<uint8_t, GCSCommandSurvey> singleByteCommands = {
    {0x04, {"Request Status Register", {}}},
    {0x05, {"Request Motion Status", {}}},
    {0x07, {"Request Controller Ready Status", {}}},
    {0x08, {"Query if Macro is Running", {}}},
    {0x18, {"Stop All Axes", {}}},
};

//multi-character commands
const std::unordered_map<std::string, GCSCommandSurvey> multiCharCommands = {
    {"*IDN?", {"Get Device Identification", {}}},
    {"ACC", {"Set Acceleration", {"AxisID", "Acceleration"}}},
    {"ACC?", {"Get Acceleration", {"AxisID"}}},
    {"ADD", {"Add and Save to variable", {"Variable", "float1", "float2"}}},
    {"BRA", {"Set Brake Activation State", {"AxisID", "BrakeState"}}}, //0 = brake off, 1 = brake on
    {"BRA?", {"Get Brake Activation State", {"AxisID"}}},
    {"CPY", {"Copy into Variable", {"Variable", "CMD?"}}},
    {"CST?", {"Get Assignment of Stages to Axes", {"AxisID"}}},
    {"CSV?", {"Get Current Syntax Version", {}}},
    {"CTO", {"Set Configuration of Trigger Output", {"TrigOutID", "CTOParam", "Value"}}},
    {"CTO?", {"Get Configuration of Trigger Output", {"TrigOutID", "CTOParam"}}},
    {"DEC", {"Set Deceleration", {"AxisID", "Deceleration"}}},
    {"DEC?", {"Get Deceleration", {"AxisID"}}},
    {"DEL", {"Delay the Command Interpreter", {"Delay"}}}, //ms
    {"DIO", {"Set Digital Output Line", {"DIOID", "Output"}}}, //1 = high, 0 = Low
    {"DIO?", {"Get Digital Output Line", {"DIOID"}}},
    {"DRC", {"Set Data Recorder Configuration", {"RecTableID", "Source", "RecOption"}}},
    {"DRC?", {"Get Data Recorder Configuration", {"RecTableID"}}},
    {"DRR?", {"Get Recorded Data Values", {"StartPoint", "NumberOfPoints","RecTableID"}}},
    {"DRT", {"Set Data Recorder Trigger Source", {"RecTableID", "TriggerSource", "Value"}}},
    {"DRT?", {"Get Data Recorder Trigger Source", {"RecTableID"}}},
    {"ERR?", {"Get Error Number", {}}},
    {"FED", {"Find Edge", {"AxisID", "EdgeID", "Param"}}},
    {"FNL", {"Fast Reference Move to Negative Limit", {"AxisID"}}},
    {"FPL", {"Fast Reference Move to Positive Limit", {"AxisID"}}},
    {"FRF", {"Fast Reference Move to Reference Switch", {"AxisID"}}},
    {"FRF?", {"Get Referencing Result", {"AxisID"}}},
    {"GOH", {"Go to Home Position", {"AxisID"}}},
    {"HDR?", {"Get All Data Recorder Options", {}}},
    {"HLP", {"Get List of Available Commands", {}}},
    {"HLT", {"Halt Motion Smoothly", {"AxisID"}}},
    {"HPA?", {"Get List of Available Parameters", {}}},
    {"JAS?", {"Query Joystick Axis Status", {"JoystickID", "JoyStickAxis"}}},
    {"JAX", {"Set Axis Controlled by Joystick", {"JoystickID", "JoyStickAxis", "AxisID"}}},
    {"JAX?", {"Get Axis Controlled by Joystick", {"JoystickID", "JoyStickAxis"}}},
    {"JAX?", {"Get Axis Controlled by Joystick", {"JoystickID", "JoyStickAxis"}}},
    {"JBS?", {"Query Joystick Button Status", {"JoystickID", "JoyStickButton"}}},
    {"JDT", {"Set Joystick Default Lookup Table", {"JoystickID", "JoyStickAxis", "LookupTableProfileType"}}}, //1 = linear (default), 2 = parabolic
    {"JLT", {"Fill Joystick Lookup Table", {"JoystickID", "JoyStickAxis", "Address", "Trajectory"}}},
    {"JLT?", {"Get Joystick Lookup Table Values", {"StartAddress", "NumberOfAddressReads", "JoystickID", "JoyStickAxis"}}},
    {"JON", {"Set Joystick Activation Status", {"JoystickID", "ActivationStatus"}}}, //0 = disable, 1 = enable
    {"JON?", {"Get Joystick Activation Status", {"JoystickID"}}},
    {"JRC", {"Jump Relatively Depending on Condition", {"JumpSize", "CMD?", "Operator", "Value"}}},
    {"LIM?", {"Indicate Limit Switches", {"AxisID"}}},
    {"MAC", {"Call Macro Function", {"Keyword", "Parameters"}}}, //Keywords = BEG, DEF, DEF?, DEL, END, ERR?, NSTART, START
    {"MAC?", {"List Macros", {"MacroName"}}},
    {"MEX", {"Stop Macro Execution due to Condition", {"CMD?", "Operator", "Value"}}},
    {"MOV", {"Set Target Position", {"AxisID", "Position"}}},
    {"MOV?", {"Get Target Position", {"AxisID"}}},
    {"MVR", {"Set Target Relative to Current Position", {"AxisID", "Distance"}}},
    {"ONT?", {"Get On Target State", {"AxisID"}}},
    {"POS", {"Set Real Position", {"AxisID", "Position"}}}, //without motion
    {"POS?", {"Get Real Position", {"AxisID"}}},
    {"RBT", {"Reboot System", {}}},
    {"RMC?", {"List Running Macros", {}}},
    {"RON", {"Set Reference Mode", {"AxisID", "ReferenceOut"}}},
    {"RON?", {"Get Reference Mode", {"AxisID"}}},
    {"RPA", {"Reset Volatile Memory Parameters", {"ItemID", "ParamID"}}},
    {"RTR", {"Set Record Table Rate", {"RecordTableRate"}}},
    {"RTR?", {"Get Record Table Rate", {}}},
    {"SAI", {"Set Current Axis Identifiers", {"AxisID", "NewIdentifier"}}},
    {"SAI?", {"Get Current Axis Identifiers", {"ALL"}}}, //If ALL sent as arguemnet deactivated axes also respond
    {"SEP", {"Set Non-Volatile Memory Parameters", {"Password", "ItemID", "ParamID", "ParamValue"}}}, // Default password = 100
    {"SPA", {"Set Volatile Memory Parameters", {"ItemID", "ParamID", "ParamValue"}}},
    {"SPA?", {"Get Volatile Memory Parameters", {"ItemID", "ParamID"}}},
    {"SRG?", {"Query Status Register Value", {"AxisID", "RegisterID"}}},
    {"STE", {"Start Step and Response Measurement", {"AxisID", "Amplitude"}}},
    {"STP", {"Stop All Axes", {}}},
    {"SVO", {"Set Motor State", {"AxisID", "MotorState"}}}, //0 = servo off, 1 = servo on
    {"SVO?", {"Get Motor State", {"AxisID"}}},
    {"TAC?", {"Tell Analog Channels", {}}},
    {"TAV", {"Get Analog Input Voltage", {"AnalogInputID"}}},
    {"TIO?", {"Tell Digital I/O Lines", {}}},
    {"TMN?", {"Get Minimum Commandable Position", {"AxisID"}}},
    {"TMX?", {"Get Maximum Commandable Position", {"AxisID"}}},
    {"TNR?", {"Get Number of Record Tables", {}}},
    {"TRO", {"Set Trigger Output State", {"TrigOutID", "TrigMode"}}}, //0 = trigger output disabled, 1 = trigger output enabled
    {"TRO?", {"Get Trigger Output State", {"TrigOutID"}}},
    {"TRS?", {"Indicate Reference Switch", {"AxisID"}}},
    {"TVI?", {"Tell Valid Character Set for Axis Identifiers", {}}},
    {"VAR", {"Set Variable Value", {"Variable", "String"}}},
    {"VAR?", {"Get Variable Value", {"Variable"}}},
    {"VEL", {"Set Velocity", {"AxisID", "Velocity"}}},
    {"VEL?", {"Get Velocity", {"AxisID"}}},
    {"VER", {"Get Versions of Firmware and Drivers", {}}},
    {"WAC", {"Wait for Condition", {"CMD?", "Operator", "Value"}}},
};

std::optional<GCSCommandSurvey> getCommandInfo(const std::string& command) {
    if (command.size() == 1) {
        uint8_t singleByte = static_cast<uint8_t>(command[0]);
        if (singleByteCommands.find(singleByte) != singleByteCommands.end()) {
            return singleByteCommands.at(singleByte);
        }
    }
    if (multiCharCommands.find(command) != multiCharCommands.end()) {
        //std::cout << "Found multi-character command: " << command << std::endl; // debugging
        return multiCharCommands.at(command);
    }
 
    //command not found
    //std::cout << "Command not found: " << command << std::endl; // debugging
    return std::nullopt;
}

std::string parseGCSCommand(const std::vector<uint8_t>& data) {
    //if command is single (non-printable) byte
    if (data.size() == 1) {
        uint8_t singleByte = data[0];
        auto commandInfo = getCommandInfo(std::string(1, static_cast<char>(singleByte)));

        if (commandInfo) {
            return "Command: #" + std::to_string(singleByte) + " | " + commandInfo->longDescription  + "\n";
        }
        return "Command: #" + std::to_string(singleByte) + "\n";
    }
    //if command is multichar
    std::string command(data.begin(), data.end());
    std::stringstream ss(command);
    std::string baseCommand;
    ss >> baseCommand;

    auto commandInfo = getCommandInfo(baseCommand);

    if (commandInfo) {
        std::string formatted = "Command: " + baseCommand + " | " + commandInfo->longDescription;

        //parse arguements
        std::vector<std::string> parsedArgs;
        std::string arg;
        while (ss >> arg) {
            parsedArgs.push_back(arg);
        }

        size_t argCount = commandInfo->argumentNames.size();
        for (size_t i = 0; i < parsedArgs.size() && i < argCount; ++i) {
            formatted += " | " + commandInfo->argumentNames[i] + ": " + parsedArgs[i];
        }

        for (size_t i = parsedArgs.size(); i < argCount; ++i) {
            formatted += " | " + commandInfo->argumentNames[i] + ": [MISSING]";
            }

        return formatted + "\n";
    }
    return "Raw command: " + command;
}

std::string cleanResponse(const std::string& response) {
    ///remove backticks (stay alive?) characters from response
    std::string cleanedResponse;
    std::copy_if(response.begin(), response.end(), std::back_inserter(cleanedResponse),
                [](char c) { return c != '`'; });
    return cleanedResponse;
}
std::optional<std::string> handleBulkInResponse(const std::vector<uint8_t>& data) {
    std::string fragment(data.begin(), data.end());
    responseBuffer += fragment;

    if (!responseBuffer.empty() && responseBuffer.back() == '\n') {
        std::string aggregatedResponse = responseBuffer;
        responseBuffer.clear();
        std::string cleanedResponse = cleanResponse(aggregatedResponse);
        std::vector<uint8_t> aggregatedData(cleanedResponse.begin(), cleanedResponse.end());
        std::string parsedResponse = parseGCSCommand(aggregatedData);
        return parsedResponse;
    }
    //incomplete response (no terminating LF)
    //bulkInByteCount += data.size();
    return std::nullopt;
}
