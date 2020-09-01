/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintLimFile.cpp: Class useful to print the Lim structure into .lim file (Dexima)
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "PrintLimFile.h"

using namespace llvm;
using namespace octantis;

namespace octantis{

PrintLimFile::PrintLimFile()
{

}

/// It prints the Constants section
void PrintLimFile::printConstants(){
    Output << "begin constants\n";
    // Printing the configuration of the Lim Unit

    Output << "end constants\n\n";
}

/// It prints the Init section
void PrintLimFile::printInit(){
    Output << "begin init\n";
    // Printing the configuration of the Lim Unit

    Output << "end init\n\n";
}

/// It prints the Lim section
void PrintLimFile::printLim(){
    Output << "begin Lim\n";
    // Printing the details of the Lim Architecture

    Output << "end Lim\n\n";

}

/// It prints the Map section
void PrintLimFile::printMap(){
    Output << "begin map";
    // Printing the map section

    Output << "end map";

}

/// It prints the Instructions section
void PrintLimFile::printInstructions(){
    Output << "begin instructions";
    // Printing the instructions section

    Output << "end instructions";

}

/// It prints the Code section
void PrintLimFile::printCode(){
    Output << "begin code";
    // Printing the code section

    Output << "end code";
}

} // End octantis namespace
