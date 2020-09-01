/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintDexFile Class: class useful for the definition of Dexima's configuration file (.dex).
//      *Note: For the syntax description, refer to Dexima's documentation.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
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

//Standard C++ Include Libraries
#include <sstream>
#include <map>
#include <string>
#include <iterator>
#include <set>

using namespace llvm;

namespace octantis{

///Class class useful for the definition of Dexima's configuration file (.dex).
class PrintDexFile
{
public:
    PrintDexFile(LiMArray *compArray, FiniteStateMachine *compFSM, raw_ostream *OStream):
        compArray(compArray), compFSM(compFSM), OStream(OStream){};

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
    void printLim();

    /// It prints the Map section
    void printMap();

    /// It prints the Instructions section
    void printInstructions();

    /// It prints the Code section
    void printCode();

    /// It generates the unique name for the each component: FUTURE IMPLEMENTATION
//    int getIndex(std::string operation);

    /// It identifies the kind of LiM row and print the corresponding code
    void identifyLIMRowAndPrint(int &currentRow, std::map<int * const, LiMArray::LiMRow>::iterator * mapIT);

    /// It is invoked when a normal memory row is declared
    void addDataRow(int &currentRow, int* const &nameRow);

    /// It prints the correct code for any BITWISE lim cell
    void printBITWISE(std::string &bitwiseOp, int &currentRow, int* const &nameRow, int* const &nameSrc);

    /// It prints the correct code for an SUM lim cell
    void printADD(int &currentRow, int* const &nameRow, int* const &nameSrc);

    /// It prints the correct code for any BITWISE lim cell with a 2to1 mux in input
    /// so that 2 input bits for the logic are available.
    void printBITWISEMux2to1(std::string &bitwiseOp, int &currentRow, int* const &nameRow, int* const &nameSrc1, int* const &nameSrc2);

    /// It prints the correct code for an SUM lim cell with a 2to1 mux in input
    /// so that 2 input bits for the logic are available.
    void printADDMux2to1(int &currentRow, int* const &nameRow, int* const &nameSrc1, int* const &nameSrc2);

    /// It prints the correct code for an SUB lim cell
//    void printSUB(int * currentRow, int **nameRow, int **nameSrc);

    /// It modifies the passed parameters (name and index) to return
    /// the requested information
    void getNameAndIndexOfSourceRow(int* const &sourceRow, std::string &sourceCellName, int &sourceCellRow);

    /// It inserts a new element inside the the namesMap
    void insertNamesMap(int* const &rowName, std::string cellName, int &cellRow);

    /// It returns the correct out pin name of opSrc row:
    /// it's useful for the identification of the output pin name
    /// of the row in case of is an Arithmetic Operation.
    void getOutPinName(std::string &opSrc, std::string &outPinName);




private:
    /// Backup variables for the LiM Array and the Operations implemented
    LiMArray * compArray;
    FiniteStateMachine * compFSM;

    ///Iterators over LiMArray and FiniteStateMachine
    ///Iterator over LiM Array
    std::map<int * const, LiMArray::LiMRow>::iterator limArrayIT;
    std::map<int, std::list<int *>>::iterator FSMIT;

    ///Iterator over the list of <int *> of the FSM
    std::list <int *>::iterator FSM_ListIT;


    /// Variables useful for the redirection of the output to .dex file
    raw_ostream * OStream;
    std::stringstream Output;

    ///The following streams are useful to define in parallel all the
    ///sections of the .dex file. At the end, they are merged with Output.
    std::stringstream OutputLiMMap;
    std::stringstream OutputInstructions;
    std::stringstream OutputCode;


    ///Map useful to keep track of the name associated to each LiM row:
    ///the name of the first MSB is stored (Here we assume that all the
    ///LiM row contains the same kind of logic)
    std::map <int * const, nameAndIndex> namesMap;

    ///Iterator over the namesMap
    std::map <int * const, nameAndIndex>::iterator namesMapIT;

    //Map to keep track of the indexes of each kind of component and its iterator
    //std::map <std::string, int> componentCnt;
    //std::map <std::string, int>::iterator componentCntIT;

};

} //End Octantis namespace

#endif // PRINTDEXFILE_H
