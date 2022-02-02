/*-------------------------------------- The Octantis Project --------------------------------------*/
/// /file
/// PrintConfig: Class useful to print the default configuration file.
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef PRINTCONFIG_H
#define PRINTCONFIG_H

#include<string>
#include<map>
#include<list>
// LLVM include files:
#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;

namespace octantis {
/// Class useful to implement a simple user-interface, letting the user fill a config file which is parsed afterwards
class PrintConfig
{
    
public:
    
    /// Default Constructor
    PrintConfig();

    /// Function to print the configuration file
    void Print();
    
    /// Function to parse the configuration file
    void ParseConfigFile();

    /// function that returns the created data structure
    std::map<std::string, std::map<std::string, std::list<std::string>>> getParsedData();

    
    void printConfigParameters();
    
    
private:

    /// Name of the config file
    std::string configFileName="Octantis.cfg";
    
    /// Data structure for the parsed data declaration
    /// |_ MacroType
    ///    |_ Field
    ///        |_ List of values / one single value
    std::map<std::string, std::map<std::string, std::list<std::string>>> parsedData;

    /// List containing the names of all the accepted Macrotypes
    const std::list<std::string> macroTypes = {"MemoryType", "SynthesisProcess", "AdvancedLiMSettings"};

    /// List containing the names of all the accepted fields for MemoryType
    const std::list<std::string> fieldsMem = {"WordLength", "MaxDimension"};

    /// List containing the names of all the accepted fields for SynthesisProcess
    const std::list<std::string> fieldsSyn = {"AdoptedOptimizations", "SchedulingAlgorithm", "OptimizedParameter"};

    /// List containing the names of all the accepted fields for AdvancedLiMSettings
    const std::list<std::string> fieldsAdv = {"ReferenceLiMArchitecture"};

    /// Iterator for a list of strings
    std::list<std::string>::iterator listIT;

    /// function that returns the list of values for a specific field
    std::list<std::string> fieldListMaker(std::string fieldData);
};
}
#endif // PRINTCONFIG_H
