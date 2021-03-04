/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintDexFile Class: class useful for the definition of Dexima's configuration file (.dex).
//      *Note: For the syntax description, refer to Dexima's documentation.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Nicola 2021 (alessio.nicola@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

#ifndef PRINTDEXFILE_H
#define PRINTDEXFILE_H

// LLVM include files:
#include "llvm/Support/raw_ostream.h"

// Octantis input files
#include "FiniteStateMachine.h"
#include "OperationsImplemented.h"
#include "LiMArray.h"

// Standard C++ Include Libraries
//#include <map>
//#include <iterator>

using namespace llvm;

namespace octantis
{

/// Class useful for the definition of Dexima's configuration file (.dex).
class PrintDexFile
{
public:
    PrintDexFile(LiMArray *compArray, FiniteStateMachine *compFSM):
        compArray(compArray), compFSM(compFSM){};

    /// It prints the .dex file
    void print();

private:
    /// Structure useful for the namesMap
    struct nameAndIndex{
        std::string name;
        int rowIndex;
    };

private:
    /// It prints the Constants section
    void printConstants();

    /// It prints the Init section
    void printInit();

    /// It prints the Lim section
    void printLiM();

    /// It prints the Map section
    void printMap();

    /// It prints the Instructions section
    void printInstructions();

    /// It prints the Code section
    void printCode(std::string &outCodeName);

    /// It identifies the kind of LiM row and print the code
    void identifyLIMRowAndPrint(int &currentRow,
        std::map<int * const, LiMArray::LiMRow>::iterator * mapIT);

    /// It is invoked when a normal memory row is declared
    void addDataRow(int &currentRow, int* const &nameRow, int * const &nameSrc);

    /// It inserts a new element inside the the namesMap
    void insertNamesMap(int* const &rowName, std::string cellName, int &cellRow);

    /// It modifies the passed parameters (name and index) to return
    /// the requested information
    void getNameAndIndexOfSourceRow(int* const &sourceRow,
        std::string &sourceCellName, int &sourceCellRow);

    /// It returns the correct out pin name of opSrc row:
    /// it's useful for the identification of the output pin name
    /// of the row in case of is an Arithmetic Operation.
    void getOutPinName(std::string &opSrc, std::string &outPinName);

    /// It prints the correct code for any BITWISE lim cell with a 2to1 mux in input
    /// so that 2 input bits for the logic are available.
    void printBITWISEMux2to1(std::string &bitwiseOp, int &currentRow,
        int* const &nameRow, int* const &nameSrc1, int* const &nameSrc2);

    /// It prints the correct code for any MIXED lim cell
    void printMIXED(std::string &bitwiseOp, int &currentRow, int * const &nameRow,
        int* const &nameSrc, std::list<std::string> &operators);

    /// It prints the correct code for any BITWISE lim cell
    void printBITWISE(std::string &bitwiseOp, int &currentRow, int* const &nameRow,
        int* const &nameSrc);

    /// It prints the correct code for an SUM lim cell with a 2to1 mux in input
    /// so that 2 input bits for the logic are available.
    void printADDMux2to1(int &currentRow, int* const &nameRow,
        int* const &nameSrc1, int* const &nameSrc2);

    /// It prints the correct code for an SUM lim cell
    void printADD(int &currentRow, int* const &nameRow, int* const &nameSrc);

    /// It merges the source file into the destination file
    /// The source file is closed if closeSource is true
    void mergeToOutFile(raw_fd_ostream *destFD, raw_fd_ostream *sourceFD,
        std::string &sourceName, bool closeSourceFD);

    /// It reads the instructions and checks if the instruction is related
    /// to the end of critical path
    void powerPathSection();

    /// It identifies the kind of LiM and prints the power and path section
    void printPowerPathSection();

    /// It prints the correct code for any BITWISE lim cell in Power and Path Section
    void printBITWISEPowerPath(int &cellIndexWrite,  int* const &nameRow,
        int* const &nameSrc);

    /// It prints the correct code for any BITWISE lim cell in Power and Path section
    /// with a 2to1 mux in input so that 2 input bits for the logic are available.
    ///     (NOTE:Check the definition of the multiplexer!)
    void printBITWISEMux2to1PowerPath(int &cellIndexWrite,
        int* const &nameRow, int* const &nameSrc1, int* const &nameSrc2);

    /// It prints the correct code for an SUM lim cell in Power and Path section
    void printADDPowerPath(int &cellIndexWrite, int* const &nameRow,
        int* const &nameSrc);

    /// It prints the correct code for an SUM lim cell in Power and Path section
    /// with a 2to1 mux in input so that 2 input bits for the logic are available.
    void printADDMux2to1PowerPath(int &cellIndexWrite, int* const &nameRow,
        int* const &nameSrc1, int* const &nameSrc2);

private:
    /// Backup variables for the LiM Array and the Operations implemented
    LiMArray *compArray;
    FiniteStateMachine *compFSM;

    /// Iterator over LiM Array
    std::map<int * const, LiMArray::LiMRow>::iterator limArrayIT;
    std::map<int, std::list<int *>>::iterator FSMIT;

    ///Iterator over the list of <int *> of the FSM
    std::list <int *>::iterator FSM_ListIT;

    ///Map useful to keep track of the name associated to each LiM row:
    ///the name of the first MSB is stored (Here we assume that all the
    ///LiM row contains the same kind of logic)
    std::map <int * const, nameAndIndex> namesMap;

    ///Iterator over the namesMap
    std::map <int * const, nameAndIndex>::iterator namesMapIT;

    ///The following file descriptors are useful to define in parallel all the
    ///sections of the .dex file. At the end, they are merged with Output.
    raw_fd_ostream *outFile;
    raw_fd_ostream *outLiMMap;
    raw_fd_ostream *outInstr;
    raw_fd_ostream *outInstrPower;
    raw_fd_ostream *outInstrPath;
    raw_fd_ostream *outCode;

    //The parallelism should refer to a configuration file
    int par=32;
};

} // namespace octantis

#endif // PRINTDEXFILE_H
