/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintCadFile Class: class useful for the generation of the Dexima CAD configuration files.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef PRINTCADFILE_H
#define PRINTCADFILE_H

// LLVM include files:
#include "llvm/Support/raw_ostream.h"

// Octantis input files
#include "Binding/FiniteStateMachine.h"
#include "Binding/LiMArray.h"
#include "Utils/OperationsImplemented.h"

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
    class PrintCadFile {
    public:
        PrintCadFile(LiMArray *compArray, FiniteStateMachine *compFSM)
                : compArray(compArray), compFSM(compFSM) {};

        /// It prints all the needed files
        void print();

    private:
        /// It merges the source file into the destination file
        /// The source file is closed if closeSource is true
        void mergeToOutFile(raw_fd_ostream *destFD, raw_fd_ostream *sourceFD,
                            std::string &sourceName, bool closeSourceFD);

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell containing a logic operator
        void printBitwiseLimcad(std::string &logicOp);

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell containing a logic operator
        /// and an input 2-to-1 mux
        void printBitwiseMux2to1Limcad(std::string &logicOp);

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell containing multitple logic operators
        void printMixedLimcad(std::list<std::string> &operators);

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell containing only a memory cell
        void printMemoryLimcad(raw_fd_ostream *const &fd, bool isMux);

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell containing a FA
        void printAddLimcad();

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell containing a FA
        /// but also an input 2-to-1 mux
        void printAddMux2to1Limcad();

        /// It prints  the first .limcad (describing cell architecture) file for a Lim Cell contained in an intermediate result row
        /// generated by Octantis. As a matter of fact, it consists in a simple memory cell
        void printResultLimcad();

        /// It prints all the needed .limcad files relative to the needed LiM Cells and the needed .irlcad files
        void printArray();

        /// It prints the .irlcad file, the one describing the intra-row logic, for the addition operation
        void printAddIrlcadFile();

        /// It prints the second .limcad file, the one describing the memory array
        void printLimcadMemFile();

        /// It prints the needed .csv file in order to specify the location of each LiM cell type inside the memory array
        void printLimcadCsvFile();

        /// It prints the needed .csv file in order to specify the intra-row logic needed for a LiM row
        void printIrlcadCsvFile();


    private:
        /// Backup variables for the LiM Array
        LiMArray *compArray;
        FiniteStateMachine *compFSM;

        /// Iterator over LiM Array
        std::map<int *const, LiMArray::LiMRow>::iterator limArrayIT;
        std::map<int, std::list<int *>>::iterator FSMIT;

        ///Iterator over the list of <int *> of the FSM
        std::list<int *>::iterator FSM_ListIT;

        // file descriptor associated to the first .limcad file, the one describing the cell architecture
        raw_fd_ostream *limcadCellFile;

        /// file descriptor referring to the .irlcad file, the one describing the intra-row logic
        raw_fd_ostream *irlcadFile;

        /// file descriptor associated to the second .limcad file, the one describing the memory array
        raw_fd_ostream *limcadMemFile;

        /// .csv file needed to specify the location of each LiM cell type inside the memory array
        raw_fd_ostream *arrayCsvFile;

        /// .csv file needed to specify the intra-row logic, if needed
        raw_fd_ostream *intraRowCsvFile;

        // The parallelism should refer to a configuration file
        int par = 32;

    };

} // namespace octantis

#endif // PRINTCADFILE_H
