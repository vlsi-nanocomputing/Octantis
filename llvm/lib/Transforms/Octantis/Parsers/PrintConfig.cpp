/*-------------------------------------- The Octantis Project --------------------------------------*/
/// /file
/// PrintConfig: Class useful to print the default configuration file.
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "PrintConfig.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace octantis;
using namespace llvm;

PrintConfig::PrintConfig() {

}

/// Function to print the configuration file
void PrintConfig::Print() {
    ofstream configFile(configFileName);

    configFile << ";*************************************************************************\n"
               << ";\t\tThe Octantis Project - Configuration file\n"
               << ";*************************************************************************\n"
               << ";automatically generated at <time> <date> <user>\n\n"
               << "[MemoryType]\n"
               << ";Constraints on the memory array\n"
               << "WordLength=32\n\n\n"
               << "[SynthesisProcess]\n"
               << ";Constraints for the synthesys process\n"
               << "AdoptedOptimizations= ...\n"
               << "SchedulingAlgorithm=ASAP\n\n\n"
               << "[AdvancedLiMSettings]\n"
               << ";Parameters to define the kind of LiM solutions to explore";

    configFile.close();

}

/// Function to parse the configuration file
void PrintConfig::ParseConfigFile() {
    // declaring the input file stream and opening the config file
    ifstream configFile(configFileName);

    // current macro type detected
    std::string currentMacroType;

    std::map<std::string, std::list<std::string>> tmpFieldMap;

    list<string> tmpValueList;

    // checking if the config file has been correctly opened
    if (configFile.is_open()) {
        // variable used to store the content of the current line
        string line;

        // while statement useful to read all the config file lines
        while (getline(configFile, line)) {
            // the remove_if method is used to remove all the unnecessary white spaces from the line
            // it return an iterator pointing to the last element of the new string, but it puts undefined characters after the end of the new string in 
            // a quantity equal to the number of white spaces removed
            // the erase method deletes these not wanted characters

            line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return std::isspace(c); }),
                       line.end());


            // checking if the line is a comment (it starts with ';') or an empty one
            if (line[0] != ';' && !(line.empty())) {
                if (line[0] == '[') {
                    // delete the first character which is "["
                    line.erase(0, 1);
                    // delete the last character which is "]"
                    line.pop_back();
                    // search for line in the list macroTypes
                    auto listIT = find(macroTypes.begin(), macroTypes.end(), line);
                    // if the element is found, hence is one of the accepted macro types
                    if (listIT != macroTypes.end()) {
                        // assigning the current macro type to the found element
                        currentMacroType = *listIT;

                        parsedData.insert(
                                std::pair<std::string, std::map<std::string, std::list<std::string>>>(currentMacroType,
                                                                                                      tmpFieldMap));
                        //errs() << currentMacroType << "\n";

                    } else {
                        cerr << "Macro Type not found.\n";
                    }
                } else {
                    // retrieve the position of the equal operator
                    size_t eqPos = line.find("=");
                    // extract the field name as a substring of line from the beginning to the position of the equal
                    string field = line.substr(0, eqPos);
                    // based on the current macro type, it is checked whether the field is among the accepted ones
                    if (currentMacroType == "MemoryType") {

                        auto listIT = find(fieldsMem.begin(), fieldsMem.end(), field);
                        // if the element is found, hence is one of the accepted fields for the MemoryType
                        if (listIT != fieldsMem.end()) {
                            // extract the value as a substring of line from the position of the equal plus one to the end of the line
                            string value = line.substr(eqPos + 1);

                            tmpValueList = fieldListMaker(value);

                            parsedData.find("MemoryType")->second.insert(
                                    std::pair<std::string, std::list<std::string>>(field, tmpValueList));
                            //errs() << "\t" << field << "\n";
                        }

                    } else if (currentMacroType == "SynthesisProcess") {

                        auto listIT = find(fieldsSyn.begin(), fieldsSyn.end(), field);
                        // if the element is found, hence is one of the accepted fields for the SynthesisProcess
                        if (listIT != fieldsSyn.end()) {
                            // extract the value as a substring of line from the position of the equal plus one to the end of the line
                            string value = line.substr(eqPos + 1);

                            tmpValueList = fieldListMaker(value);

                            parsedData.find("SynthesisProcess")->second.insert(
                                    pair<string, list<string>>(field, tmpValueList));
                            //errs() << "\t" << field << "\n";
                        }

                    } else if (currentMacroType == "AdvancedLiMSettings") {

                        auto listIT = find(fieldsAdv.begin(), fieldsAdv.end(), field);
                        // if the element is found, hence is one of the accepted fields for the AdvancedLiMSettings
                        if (listIT != fieldsAdv.end()) {
                            // extract the value as a substring of line from the position of the equal plus one to the end of the line
                            string value = line.substr(eqPos + 1);

                            tmpValueList = fieldListMaker(value);

                            parsedData.find("AdvancedLiMSettings")->second.insert(
                                    pair<string, list<string>>(field, tmpValueList));
                            //errs() << "\t" << field << "\n";
                        }

                    } else {

                        cerr << "Macro Type not found, the subsequent fields are not being considered.\n";

                    }
                }
            }
        }
    } else {
        // print an error in case the file has not been opened correctly
        cerr << "Could not open config file for reading.\n";
    }
}

///It parses the command line
void PrintConfig::parseCommandLine() {

    //cl::opt<bool> debugMode("debugMode", cl::desc("Specify debug mode"));
    //errs() << debugMode << "\n\n";


}

list<string> PrintConfig::fieldListMaker(string fieldData) {
    list<string> tmpList;
    string subValue;
    size_t commaPos;
    size_t currentPos = 0;

    if (fieldData[0] == '[') {

        fieldData.erase(0, 1);
        fieldData.pop_back();

        commaPos = fieldData.find(",", currentPos);
        while (commaPos != NULL) {
            subValue = fieldData.substr(currentPos, commaPos);
            tmpList.push_back(subValue);
            currentPos = commaPos + 1;
            commaPos = fieldData.find(",", currentPos);
        }
    } else {
        tmpList.push_back(fieldData);
    }

    return tmpList;
}

void PrintConfig::printConfigParameters() {

    for (auto mapIT = parsedData.begin(); mapIT != parsedData.end(); ++mapIT) {
        errs() << mapIT->first << "\n";
        for (auto mapIT2 = (mapIT->second).begin(); mapIT2 != (mapIT->second).end(); ++mapIT2) {
            errs() << "\t" << mapIT2->first << "\n";
            for (auto listIT = (mapIT2->second).begin(); listIT != (mapIT2->second).end(); ++listIT) {
                errs() << "\t\t" << *listIT << "\n";
            }
        }
    }
}

int PrintConfig::getWordLength() {
    return std::stoi(*(parsedData.at("MemoryType").at("WordLength").begin()));
}

std::string PrintConfig::getSchedulingAlgorithm() {
    return *(parsedData.at("SynthesisProcess").at("SchedulingAlgorithm").begin());
}

std::string PrintConfig::getOptimizationTarget() {
    return *(parsedData.at("SynthesisProcess").at("AdoptedOptimizations").begin());
}
