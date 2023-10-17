/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PointerInfoTable Class: class useful to collect info about array-related pointers in the code                  
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "PointerInfoTable.h"

//LLVM Include Files
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

#include "Utils/AccessPattern.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Default constructor
PointerInfoTable::PointerInfoTable() {

}

///Function useful to return the infos about a specific pointer
PointerInfoTable::pointerInfoStruct PointerInfoTable::getPointerInfo(int *pointer) {
    pointerInfoMapIT = pointerInfoMap.find(pointer);

    return pointerInfoMapIT->second;
}

///Function useful to modify the PointerInfo field related to an element in PointerInfoMap
void PointerInfoTable::modifyPointerInfo(int *pointer, PointerInfoTable::pointerInfoStruct pointerInfo) {
    pointerInfoMapIT = pointerInfoMap.find(pointer);
    pointerInfoMapIT->second = pointerInfo;
}

///Function useful to insert in PointerInfoMap a new element
void PointerInfoTable::insertPointerInfo(int *pointer, PointerInfoTable::pointerInfoStruct pointerInfo) {
    pointerInfoMap.insert(std::pair<int *, PointerInfoTable::pointerInfoStruct>(pointer, pointerInfo));
}

/*-----------------------------DEBUG FUNCTIONS-------------------------------*/
void PointerInfoTable::printPointerInfoTable() {
    errs() << "Printing AIT\n";

    for (pointerInfoMapIT = pointerInfoMap.begin(); pointerInfoMapIT != pointerInfoMap.end(); ++pointerInfoMapIT) {
        errs() << "\tpointer: " << pointerInfoMapIT->first << "\n"
               << "\t\tloopIterator: " << (pointerInfoMapIT->second).loopIterator << "\n"
               << "\t\tarrayRows: " << (pointerInfoMapIT->second).arrayRows << "\n"
               << "\t\tarrayCols: " << (pointerInfoMapIT->second).arrayCols << "\n"
               << "\t\tsetRows: " << (pointerInfoMapIT->second).setRows << "\n"
               << "\t\tsetCols: " << (pointerInfoMapIT->second).setCols << "\n"
               << "\t\tspacingInSubsetX: " << (pointerInfoMapIT->second).spacingInSubsetX << "\n"
               << "\t\tspacingInSubsetY: " << (pointerInfoMapIT->second).spacingInSubsetY << "\n"
               << "\t\tspacingX: " << (pointerInfoMapIT->second).spacingX << "\n"
               << "\t\tspacingY: " << (pointerInfoMapIT->second).spacingY << "\n"
               << "\t\toffsetX: " << (pointerInfoMapIT->second).offsetX << "\n"
               << "\t\toffsetY: " << (pointerInfoMapIT->second).offsetY << "\n"
               << "\t\tstopX: " << (pointerInfoMapIT->second).stopX << "\n"
               << "\t\tstopY: " << (pointerInfoMapIT->second).stopY << "\n"
               << "\t\tfinal zeros: " << (pointerInfoMapIT->second).pointerAccessPattern.getFinalZeros() << "\n"
               << "\t\tinit zeros: " << (pointerInfoMapIT->second).pointerAccessPattern.getInitZeros() << "\n"
               << "\t\tnumberOfSubsets: " << (pointerInfoMapIT->second).numberOfSubsets << "\n"
               << "\t\tnumberOfSubsetsElements: " << (pointerInfoMapIT->second).numberOfSubsetsElements << "\n"

               << "\t\tAPM: " << (pointerInfoMapIT->second).pointerAccessPattern.getApmType() << "\n"

               << "\t\tfirstIdx: " << (((pointerInfoMapIT->second).firstIdxInfo).iterators).front() << "\n"
               << "\t\tsecondIdx: " << (((pointerInfoMapIT->second).secondIdxInfo).iterators).front() << "\n";

        errs() << "\t\tArray access pattern\n";

        for (int i = 0; i < ((pointerInfoMapIT->second).pointerAccessPattern).getRows(); ++i) {
            errs() << "\t\t\t";
            for (int j = 0; j < ((pointerInfoMapIT->second).pointerAccessPattern).getCols(); ++j) {
                errs() << (pointerInfoMapIT->second).pointerAccessPattern(i, j) << " ";
            }
            errs() << "\n";
        }


        errs() << "\t\tArray access pattern constant\n";

        for (int i = 0; i < ((pointerInfoMapIT->second).pointerAccessPatternConstant).getRows(); ++i) {
            errs() << "\t\t\t";
            for (int j = 0; j < ((pointerInfoMapIT->second).pointerAccessPatternConstant).getCols(); ++j) {
                errs() << (pointerInfoMapIT->second).pointerAccessPatternConstant(i, j) << " ";
            }
            errs() << "\n";
        }
    }
}
/*-----------------------------DEBUG FUNCTIONS-------------------------------*/