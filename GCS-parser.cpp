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

//single-byte queries
const std::unordered_map<uint8_t, GCSCommandSurvey> singleByteCommands = {
    {0x18, {"Stop All Axes", {}}},
};

//single-byte queries
const std::unordered_map<uint8_t, GCSCommandSurvey> singleByteQueries = {
    {0x04, {"Request Status Register", {}}},
    {0x05, {"Request Motion Status", {}}},
    {0x07, {"Request Controller Ready Status", {}}},
    {0x08, {"Query if Macro is Running", {}}},  
};

//multi-character commands
const std::unordered_map<std::string, GCSCommandSurvey> multiCharCommands = {
    {"ACC", {"Set Acceleration", {"AxisID", "Acceleration"}}},
    {"ADD", {"Add and Save to variable", {"Variable", "float1", "float2"}}},
    {"BRA", {"Set Brake Activation State", {"AxisID", "BrakeState"}}}, //0 = brake off, 1 = brake on
    {"CPY", {"Copy into Variable", {"Variable", "CMD?"}}},
    {"CTO", {"Set Configuration of Trigger Output", {"TrigOutID", "CTOParam", "Value"}}},
    {"DEC", {"Set Deceleration", {"AxisID", "Deceleration"}}},
    {"DEL", {"Delay the Command Interpreter", {"Delay"}}}, //ms
    {"DIO", {"Set Digital Output Line", {"DIOID", "Output"}}}, //1 = high, 0 = Low
    {"DRC", {"Set Data Recorder Configuration", {"RecTableID", "Source", "RecOption"}}},
    {"DRT", {"Set Data Recorder Trigger Source", {"RecTableID", "TriggerSource", "Value"}}},
    {"FED", {"Find Edge", {"AxisID", "EdgeID", "Param"}}},
    {"FNL", {"Fast Reference Move to Negative Limit", {"AxisID"}}},
    {"FPL", {"Fast Reference Move to Positive Limit", {"AxisID"}}},
    {"FRF", {"Fast Reference Move to Reference Switch", {"AxisID"}}},
    {"GOH", {"Go to Home Position", {"AxisID"}}},
    {"HLP", {"Get List of Available Commands", {}}},
    {"HLT", {"Halt Motion Smoothly", {"AxisID"}}},
    {"JAX", {"Set Axis Controlled by Joystick", {"JoystickID", "JoyStickAxis", "AxisID"}}},
    {"JDT", {"Set Joystick Default Lookup Table", {"JoystickID", "JoyStickAxis", "LookupTableProfileType"}}}, //1 = linear (default), 2 = parabolic
    {"JLT", {"Fill Joystick Lookup Table", {"JoystickID", "JoyStickAxis", "Address", "Trajectory"}}},
    {"JON", {"Set Joystick Activation Status", {"JoystickID", "ActivationStatus"}}}, //0 = disable, 1 = enable
    {"JON?", {"Get Joystick Activation Status", {"JoystickID"}}},
    {"JRC", {"Jump Relatively Depending on Condition", {"JumpSize", "CMD?", "Operator", "Value"}}},
    {"MAC", {"Call Macro Function", {"Keyword", "Parameters"}}}, //Keywords = BEG, DEF, DEF?, DEL, END, ERR?, NSTART, START
    {"MEX", {"Stop Macro Execution due to Condition", {"CMD?", "Operator", "Value"}}},
    {"MOV", {"Set Target Position", {"AxisID", "Position"}}},
    {"MVR", {"Set Target Relative to Current Position", {"AxisID", "Distance"}}},
    {"POS", {"Set Real Position", {"AxisID", "Position"}}}, //without motion
    {"POS?", {"Get Real Position", {"AxisID"}}},
    {"RBT", {"Reboot System", {}}},
    {"RON", {"Set Reference Mode", {"AxisID", "ReferenceOut"}}},
    {"RON?", {"Get Reference Mode", {"AxisID"}}},
    {"RPA", {"Reset Volatile Memory Parameters", {"ItemID", "ParamID"}}},
    {"RTR", {"Set Record Table Rate", {"RecordTableRate"}}},
    {"RTR?", {"Get Record Table Rate", {}}},
    {"SAI", {"Set Current Axis Identifiers", {"AxisID", "NewIdentifier"}}},
    {"SEP", {"Set Non-Volatile Memory Parameters", {"Password", "ItemID", "ParamID", "ParamValue"}}}, // Default password = 100
    {"SPA", {"Set Volatile Memory Parameters", {"ItemID", "ParamID", "ParamValue"}}},
    {"STE", {"Start Step and Response Measurement", {"AxisID", "Amplitude"}}},
    {"STP", {"Stop All Axes", {}}},
    {"SVO", {"Set Motor State", {"AxisID", "MotorState"}}}, //0 = servo off, 1 = servo on
    {"TAV", {"Get Analog Input Voltage", {"AnalogInputID"}}},
    {"TRO", {"Set Trigger Output State", {"TrigOutID", "TrigMode"}}}, //0 = trigger output disabled, 1 = trigger output enabled
    {"VAR", {"Set Variable Value", {"Variable", "String"}}},
    {"VEL", {"Set Velocity", {"AxisID", "Velocity"}}},
    {"VER", {"Get Versions of Firmware and Drivers", {}}},
    {"WAC", {"Wait for Condition", {"CMD?", "Operator", "Value"}}},
};

//multi-character queries
const std::unordered_map<std::string, GCSCommandSurvey> multiCharQueries = {
    {"*IDN?", {"Get Device Identification", {}}},
    {"ACC?", {"Get Acceleration", {"AxisID"}}},
    {"BRA?", {"Get Brake Activation State", {"AxisID"}}},
    {"CST?", {"Get Assignment of Stages to Axes", {"AxisID"}}},
    {"CSV?", {"Get Current Syntax Version", {}}},
    {"CTO?", {"Get Configuration of Trigger Output", {"TrigOutID", "CTOParam"}}},
    {"DEC?", {"Get Deceleration", {"AxisID"}}},
    {"DIO?", {"Get Digital Output Line", {"DIOID"}}},
    {"DRC?", {"Get Data Recorder Configuration", {"RecTableID"}}},
    {"DRR?", {"Get Recorded Data Values", {"StartPoint", "NumberOfPoints","RecTableID"}}},
    {"DRT?", {"Get Data Recorder Trigger Source", {"RecTableID"}}},
    {"ERR?", {"Get Error Number", {}}},
    {"FRF?", {"Get Referencing Result", {"AxisID"}}},
    {"HDR?", {"Get All Data Recorder Options", {}}},
    {"HPA?", {"Get List of Available Parameters", {}}},
    {"JAS?", {"Query Joystick Axis Status", {"JoystickID", "JoyStickAxis"}}},
    {"JAX?", {"Get Axis Controlled by Joystick", {"JoystickID", "JoyStickAxis"}}},
    {"JBS?", {"Query Joystick Button Status", {"JoystickID", "JoyStickButton"}}},
    {"JLT?", {"Get Joystick Lookup Table Values", {"StartAddress", "NumberOfAddressReads", "JoystickID", "JoyStickAxis"}}},
    {"LIM?", {"Indicate Limit Switches", {"AxisID"}}},
    {"MAC?", {"List Macros", {"MacroName"}}},
    {"MOV?", {"Get Target Position", {"AxisID"}}},
    {"ONT?", {"Get On Target State", {"AxisID"}}},
    {"RMC?", {"List Running Macros", {}}},
    {"SAI?", {"Get Current Axis Identifiers", {"ALL"}}}, //If ALL sent as arguemnet deactivated axes also respond
    {"SPA?", {"Get Volatile Memory Parameters", {"ItemID", "ParamID"}}},
    {"SRG?", {"Query Status Register Value", {"AxisID", "RegisterID"}}},
    {"SVO?", {"Get Motor State", {"AxisID"}}},
    {"TAC?", {"Tell Analog Channels", {}}},
    {"TIO?", {"Tell Digital I/O Lines", {}}},
    {"TMN?", {"Get Minimum Commandable Position", {"AxisID"}}},
    {"TMX?", {"Get Maximum Commandable Position", {"AxisID"}}},
    {"TNR?", {"Get Number of Record Tables", {}}},
    {"TRO?", {"Get Trigger Output State", {"TrigOutID"}}},
    {"TRS?", {"Indicate Reference Switch", {"AxisID"}}},
    {"TVI?", {"Tell Valid Character Set for Axis Identifiers", {}}},
    {"VAR?", {"Get Variable Value", {"Variable"}}},
    {"VEL?", {"Get Velocity", {"AxisID"}}},
};

std::optional<GCSCommandSurvey> getCommandInfo(const std::string& command) {
    if (command.size() == 1) {
        uint8_t singleByte = static_cast<uint8_t>(command[0]);
        if (singleByteCommands.find(singleByte) != singleByteCommands.end()) {
            return singleByteCommands.at(singleByte);
        }
        if (singleByteQueries.find(singleByte) != singleByteQueries.end()) {
            return singleByteQueries.at(singleByte);
        }
    }
    if (command.find('?') != std::string::npos) {
        if (multiCharQueries.find(command) != multiCharQueries.end()) {
            return multiCharQueries.at(command);
        }
    }
    if (multiCharCommands.find(command) != multiCharCommands.end()) {
        return multiCharCommands.at(command);
    }
    //command not found
    //std::cout << "Command not found: " << command << std::endl; // debugging
    return std::nullopt;
}

std::string processCommand(
    const std::string& command,
    const std::string& controllerAddress,
    const std::string& hostAddress,
    const std::optional<GCSCommandSurvey>& commandInfo,
    const std::vector<std::string>& tokens,
    size_t tokenIndex
) {
    if (commandInfo) {
        std::string formatted = "Command: " + command + " | " + commandInfo->longDescription;
    
        for (size_t i = tokenIndex; i < tokens.size(); ++i) {
            size_t argIndex = i - tokenIndex;
            if (argIndex < commandInfo->argumentNames.size()) {
                formatted += " | " + commandInfo->argumentNames[argIndex] + ": " + tokens[i];
            } else {
                formatted += " | ExtraArg" + std::to_string(argIndex +1) + ": " + tokens[i];
            }
        }
        return formatted + "\n";
    }
    return "Unrecognized command: " + command + "\n";
}

std::string parseGCSCommand(const std::vector<uint8_t>& data) {
    std::string command(data.begin(), data.end());
    std::stringstream ss(command);
    std::string token;
    std::vector<std::string> tokens;

    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return "Unrecogonized command: no data\n";
    }
    //check if controller or contoller host addresses supplied
    size_t tokenIndex = 0;
    std::string controllerAddress, hostAddress;
    //Controller Address
    if (std::all_of(tokens[0].begin(), tokens[0].end(), ::isdigit)) {
        controllerAddress = tokens[tokenIndex++];
    }
    //Host Address
    if (tokens.size() > tokenIndex && std::all_of(tokens[tokenIndex].begin(), tokens[tokenIndex].end(), ::isdigit)) {
        hostAddress = tokens[tokenIndex++];
    }

    //remaining tokens
    //single byte commands
    if (tokens.size() == 1 + tokenIndex && tokens[tokenIndex].size() == 1) {
        uint8_t singleByte = static_cast<uint8_t>(tokens[tokenIndex][0]);
        auto commandInfo = getCommandInfo(std::string(1, static_cast<char>(singleByte)));
        return processCommand("#" + std::to_string(singleByte), controllerAddress, hostAddress, commandInfo, tokens, tokenIndex + 1);
    }
    //multicharacter commands
    if (tokens.size() > tokenIndex) {
        std::string commandMnemonic = tokens[tokenIndex++];
        auto commandInfo = getCommandInfo(commandMnemonic);
        return processCommand(commandMnemonic, controllerAddress, hostAddress, commandInfo, tokens, tokenIndex);
    }
    //command not recognized
    return "Unrecogonized command: " + command + "\n";
}

std::string cleanResponse(const std::string& response) {
    ///remove backticks (stay alive?) characters from response
    std::string cleanedResponse;
    std::copy_if(response.begin(), response.end(), std::back_inserter(cleanedResponse),
                [](char c) { return c != '`'; });
    return cleanedResponse;
}

std::string parseGCSResponse(const std::vector<uint8_t>& data) {
    std::string response(data.begin(), data.end());
    response = cleanResponse(response);
    return "Response: " + response;
}

std::optional<std::string> handleBulkInResponse(const std::vector<uint8_t>& data) {
    std::string responseFragment(data.begin(), data.end());
    static std::string aggregatedResponse;
    
    //if simple stay-awake command.  Do not aggregate.
    responseFragment.erase(std::remove_if(responseFragment.begin(), responseFragment.end(),
        [](unsigned char c) { return c < 17 && c != '\n'; }), responseFragment.end());

    if (responseFragment == "`") {
        return std::nullopt;
    }

    aggregatedResponse += responseFragment;
    //multi-line response are terminated with a LF character.  Intermediate lines with a white space followed by LF
    if (!aggregatedResponse.empty() && aggregatedResponse.back() == '\n') {
        if (aggregatedResponse.size() > 1 && aggregatedResponse[aggregatedResponse.size() - 2] != ' ') {
            //std::string cleanedResponse = cleanResponse(aggregatedResponse);
            std::vector<uint8_t> aggregatedData(aggregatedResponse.begin(), aggregatedResponse.end());
            std::string parsedResponse = parseGCSResponse(aggregatedData);
            aggregatedResponse.clear();
            return parsedResponse;
        }
        //incomplete response (no terminating LF without preceding whitespace)
    }
    //continue aggregating
    return std::nullopt;
}
