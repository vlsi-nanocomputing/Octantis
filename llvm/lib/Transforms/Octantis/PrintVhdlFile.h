/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintVhdlFile Class: class useful for the generation of a vhdl description of the generated LiM structure.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef PRINTVHDLFILE_H
#define PRINTVHDLFILE_H

// LLVM include files:
#include "llvm/Support/raw_ostream.h"

// Octantis input files
#include "FiniteStateMachine.h"
#include "LiMArray.h"
#include "OperationsImplemented.h"

// Standard C++ Include Libraries
#include <iterator>
#include <map>
#include <list>
#include <set>
#include <sstream>
#include <string>

// For debug purposes
#include <fstream>

using namespace llvm;

namespace octantis {

/// Class useful for the generation of a VHDL description of the generated LiM structure.
class PrintVhdlFile {
public:
  PrintVhdlFile(LiMArray *compArray, FiniteStateMachine *compFSM, bool debugMode)
      : compArray(compArray), compFSM(compFSM), entries(compArray->getDimensions()), debugMode(debugMode){};

  /// It prints the .vhd file for both the memory entity and its testbench
  void print();

private:
  /// It merges the source file into the destination file
  /// The source file is closed if closeSource is true
  void mergeToOutFile(raw_fd_ostream *destFD, raw_fd_ostream *sourceFD,
                      std::string &sourceName, bool closeSourceFD);

  /// Function useful in order to add the memory array types and declaration in the first part of the architecture section
  void printMemArraysDec(raw_fd_ostream *const &sigFd);

  /// It prints the series of processes needed in order to properly carry out writing and reading operations, but also the entire algorithm
  void printProcesses(raw_fd_ostream *const &compFd, raw_fd_ostream *const &sigFd);

  /// It returns true if the input LiMRow is has an input which is not the output of another LiMRow
  bool isInputRow(int *const &rowName);

  /// It is useful to get the total number of data that must be retrieved from the outside and written into the memory before starting the algoritm execution
  std::string getNumberInputs();

  /// It is useful in order to know the amount of data that can be read once the algorithm has finished
  std::string getNumberOutputs();

  /// It prints the correct code for any BITWISE lim cell
  void printBitwise(int *const &rowName, int *const &srcRow1,
                    int *const &srcRow2, std::string &inputConnectionType,
                    std::string &bitwiseOp, raw_fd_ostream *const &sigFd,
                    raw_fd_ostream *const &compFd);

  /// It prints the correct code for any MIXED lim cell
  void printMixed(std::string &bitwiseOp, int *const &rowName,
                  int *const &srcRow1, int *const &srcRow2,
                  std::string &limRowComp, std::list<std::string> &operators,
                  raw_fd_ostream *const &sigFd, raw_fd_ostream *const &compFd);

  /// It prints the correct code for any BITWISE lim cell with a 2to1 mux in
  /// input
  /// so that 2 input bits for the logic are available.
  void printBitwiseMux2to1(std::string &bitwiseOp, int *const &rowName,
                           int *const &srcRow1, std::string &inputConnectionType1, std::string &inputConnectionType2,
                           int *const &srcRow2, int *const &srcRow3,
                           raw_fd_ostream *const &sigFd,
                           raw_fd_ostream *const &compFd);

  /// It prints the correct code for an SUM lim cell with a 2to1 mux in input
  /// so that 2 input bits for the logic are available.
  void printAddMux2to1(int *const &rowName, int *const &srcRow1,
                       std::string &inputConnectionType1, std::string &inputConnectionType2, int *const &srcRow2,
                       int *const &srcRow3, raw_fd_ostream *const &sigFd,
                       raw_fd_ostream *const &compFd);

  /// It prints the correct code for an SUM lim cell
  void printAdd(int *const &rowName, int *const &srcRow1, int *const &srcRow2, std::string &inputConnectionType,
                raw_fd_ostream *const &sigFd, raw_fd_ostream *const &compFd);

  /// It prints the correct code for a right shift lim cell
  void printShiftRight(int *const &rowName, int *const &srcRow1, int *const &srcRow2,
                std::string &limRowComp, raw_fd_ostream *const &sigFd,
                raw_fd_ostream *const &compFd);

  /// It prints the imported libraries declaration part
  void printLib();

  /// It is the function devoted to the printing of the ENTITY declaration part
  /// inside the .vhd file
  void printEntity();

  /// It is the function devoted to the printing of the ARCHITECTURE part
  /// inside the .vhd file
  void printArchitecture();

  /// It prints the testbench for the .vhd LiM component created
  void printTestbench();
private:
  /// Backup variables for the LiM Array
  LiMArray *compArray;
  FiniteStateMachine *compFSM;

  /// Iterator over LiM Array
  std::map<int *const, LiMArray::LiMRow>::iterator limArrayIT;
  std::map<int, std::list<int *>>::iterator FSMIT;

  ///Iterator over the list of <int *> of the FSM
  std::list <int *>::iterator FSM_ListIT;

  /// The following file descriptors are used in order to define different parts
  /// of the final .vhd file and will be merged in outFile, which is the final
  /// output
  raw_fd_ostream *outFile;

  /// file descriptor taking care of the components declaration part inside the
  /// ARCHITECTURE part, before the BEGIN statement
  raw_fd_ostream *compDec;

  /// file descriptor taking care of the signals declaration part inside the
  /// ARCHITECTURE part, before the BEGIN statement
  raw_fd_ostream *sigDec;

  /// file descriptor taking care of the components instantiation part inside
  /// the ARCHITECTURE part, after the BEGIN statement
  raw_fd_ostream *compInst;

  raw_fd_ostream *tbFile;

  // The parallelism should refer to a configuration file
  int par = 8;

  // The number of words inside the memory
  int entries;

  std::list<int*> resultRowList;


  bool debugMode;
};

} // namespace octantis

#endif // PRINTVHDLFILE_H
