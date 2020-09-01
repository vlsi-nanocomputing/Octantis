/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintLimFile.h: Class useful to print the Lim structure into .lim file (Dexima)
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef PRINTLIMFILE_H
#define PRINTLIMFILE_H

#include "llvm/Support/raw_ostream.h" // What is it useful for?
#include "llvm/IR/Instructions.h"
#include "LimUnit.h"
//Include also the Lim module we have to print

using namespace llvm;
namespace octantis {

/// PrintLimFile: Class useful for printing the LiM architecture
/// into .lim Dexima's configuration file.
class PrintLimFile
{
public:
    PrintLimFile();

private:

    /// It prints the Constants section
    void printConstants();

    /// It prints the Init section
    void printInit();

    /// It prints the Lim section
    void printLim();

    /// It prints the Map section
    void printMap();

    /// It prints the Instructions section
    void printInstructions();

    /// It prints the Code section
    void printCode();

    const LimUnit *lim;
    std::stringstream Output;

};

} // End octantis namespace
#endif // PRINTLIMFILE_H
