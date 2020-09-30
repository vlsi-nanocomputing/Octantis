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

using namespace std;

PrintConfig::PrintConfig()
{

}

/// Function to print the configuration file
void PrintConfig::print()
{
    ofstream configFile (configFileName);

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
