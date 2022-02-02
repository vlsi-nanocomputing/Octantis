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

#include "AccessPattern.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Default constructor
PointerInfoTable::PointerInfoTable()
{

}

///It detects the array access pattern
PointerInfoTable::arrayAccessPattern PointerInfoTable::detectPointerAccessPattern(AccessPattern pointerAccessPattern){

    PointerInfoTable::arrayAccessPattern tmpPattern = undefined;

    int iInitPos, jInitPos;
    bool firstEl = true;
    std::vector<int> incrementVectorRowMaj, incrementVectorColMaj, incrementVectorRowDiag, incrementVectorColDiag;

    bool isRowMajor = true;
    bool isColMajor = true;
    bool isRowDiag = true;
    bool isColDiag = true;

    int patternRows = pointerAccessPattern.getRows();
    int patternCols = pointerAccessPattern.getCols();

    if(patternRows > 1){
        for(int i = 0; i < patternRows; ++i){
            for(int j = 0; j < patternCols; ++j){

                if(pointerAccessPattern(i,j) != 0 && firstEl){

                    iInitPos = i;
                    jInitPos = j;
                    firstEl = false;

                }else if(pointerAccessPattern(i,j) != 0 && !firstEl){

                    if(i - iInitPos > 0 && j - jInitPos > 0){
                        incrementVectorRowMaj.push_back(pointerAccessPattern(i,j));
                    }else{
                        isRowMajor = false;
                    }

                }
            }
        }

        firstEl = true;

        for(int i = 0; i < patternRows; ++i){
            for(int j = 0; j < patternCols; ++j){

                if(pointerAccessPattern(i,j) != 0 && firstEl){

                    iInitPos = i;
                    jInitPos = j;
                    firstEl = false;

                }else if(pointerAccessPattern(i,j) != 0 && !firstEl){

                    if(i - iInitPos > 0 && j - jInitPos < 0){
                        incrementVectorColMaj.push_back(pointerAccessPattern(i,j));
                    }else{
                        isColMajor = false;
                    }

                }
            }
        }

        /*
    firstEl = true;

    while(isRowDiag){
        for(int i = 0; i < pointerAccessPattern.getRows(); ++i){
            for(int j = 0; j < pointerAccessPattern.getCols(); ++j){

                if(pointerAccessPattern(i,j) != 0 && firstEl){

                    iInitPos = i;
                    jInitPos = j;
                    firstEl = false;

                }else if(pointerAccessPattern(i,j) != 0 && !firstEl){

                    if(){
                        incrementVectorColMaj.push_back(pointerAccessPattern(i,j));
                    }else{
                        isRowDiag = false;
                    }

                }
            }
        }
    }

    firstEl = true;

    while(isColDiag){
        for(int i = 0; i < pointerAccessPattern.getRows(); ++i){
            for(int j = 0; j < pointerAccessPattern.getCols(); ++j){

                if(pointerAccessPattern(i,j) != 0 && firstEl){

                    iInitPos = i;
                    jInitPos = j;
                    firstEl = false;

                }else if(pointerAccessPattern(i,j) != 0 && !firstEl){

                    if(){
                        incrementVectorColMaj.push_back(pointerAccessPattern(i,j));
                    }else{
                        isColDiag = false;
                    }

                }
            }
        }
    }*/

    }else{

        for(int j = 0; j < patternCols; ++j){

            if(pointerAccessPattern(0,j) != 0){

                if(j % 2 == 0){
                    if(pointerAccessPattern(0,j) > 0){
                        return rowMajorPosColPos;
                    }else{
                        return rowMajorNegColPos;
                    }
                }else{
                    if(pointerAccessPattern(0,j) > 0){
                        return colMajorPosRowPos;
                    }else{
                        return colMajorNegRowPos;
                    }
                }

            }

        }

    }
    
    if(isRowMajor){
        if(incrementVectorRowMaj.front() > 0 && incrementVectorRowMaj.back() > 0){
            tmpPattern = rowMajorPosColPos;
        }else if(incrementVectorRowMaj.front() > 0 && incrementVectorRowMaj.back() < 0){
            tmpPattern = rowMajorPosColNeg;
        }else if(incrementVectorRowMaj.front() < 0 && incrementVectorRowMaj.back() > 0){
            tmpPattern = rowMajorNegColPos;
        }else if(incrementVectorRowMaj.front() < 0 && incrementVectorRowMaj.back() < 0){
            tmpPattern = rowMajorNegColNeg;
        }

    }else if(isColMajor){

        if(incrementVectorColMaj.front() > 0 && incrementVectorColMaj.back() > 0){
            tmpPattern = colMajorPosRowPos;
        }else if(incrementVectorColMaj.front() > 0 && incrementVectorColMaj.back() < 0){
            tmpPattern = colMajorPosRowNeg;
        }else if(incrementVectorColMaj.front() < 0 && incrementVectorColMaj.back() > 0){
            tmpPattern = colMajorNegRowPos;
        }else if(incrementVectorColMaj.front() < 0 && incrementVectorColMaj.back() < 0){
            tmpPattern = colMajorNegRowNeg;
        }

    }else if(isRowDiag){

    }else if(isColDiag){

    }

    return tmpPattern;

}

///Function useful to return the infos about a specific pointer
PointerInfoTable::pointerInfoStruct PointerInfoTable::getPointerInfo(int * pointer){
    pointerInfoMapIT = pointerInfoMap.find(pointer);

    return pointerInfoMapIT->second;
}

///Function useful to modify the PointerInfo field related to an element in PointerInfoMap
void PointerInfoTable::modifyPointerInfo(int * pointer, PointerInfoTable::pointerInfoStruct pointerInfo){
    pointerInfoMapIT = pointerInfoMap.find(pointer);
    pointerInfoMapIT->second = pointerInfo;
}

///Function useful to insert in PointerInfoMap a new element
void PointerInfoTable::insertPointerInfo(int * pointer, PointerInfoTable::pointerInfoStruct pointerInfo){
    pointerInfoMap.insert(std::pair<int *, PointerInfoTable::pointerInfoStruct> (pointer, pointerInfo));
}

/*-----------------------------DEBUG FUNCTIONS-------------------------------*/
void PointerInfoTable::printPointerInfoTable(){
    errs()<< "Printing AIT\n";

    for(pointerInfoMapIT = pointerInfoMap.begin(); pointerInfoMapIT != pointerInfoMap.end(); ++pointerInfoMapIT){
        errs()<< "\tpointer: " << pointerInfoMapIT->first
                << "\tloopIterator: " << (pointerInfoMapIT->second).loopIterator 
                << "\tnumber of sets: " << (pointerInfoMapIT->second).numberOfSets 
                << "\tsetRows: " << (pointerInfoMapIT->second).setRows 
                << "\tsetCols: " << (pointerInfoMapIT->second).setCols 
                << "\trowFirst: " << (pointerInfoMapIT->second).rowFirst
                << "\tcolFirst: " << (pointerInfoMapIT->second).colFirst 
                << "\trowFirstSet: " << (pointerInfoMapIT->second).rowFirstSet 
                << "\tcolFirstSet: " << (pointerInfoMapIT->second).colFirstSet 
                << "\tfirstIdx: " << (((pointerInfoMapIT->second).firstIdxInfo).iterators).front()
                << "\tsecondIdx: " << (((pointerInfoMapIT->second).secondIdxInfo).iterators).front() << "\n";

        errs() << "Array access pattern 1\n";

        for(int i = 0; i < ((pointerInfoMapIT->second).pointerAccessPattern).getRows(); ++i){
            for(int j = 0; j < ((pointerInfoMapIT->second).pointerAccessPattern).getCols(); ++j){
                errs() << (pointerInfoMapIT->second).pointerAccessPattern(i,j) << " ";
            }
            errs() << "\n";
        }


        errs() << "Array access pattern constant 1\n";
        
        for(int i = 0; i < ((pointerInfoMapIT->second).pointerAccessPatternConstant).getRows(); ++i){
            for(int j = 0; j < ((pointerInfoMapIT->second).pointerAccessPatternConstant).getCols(); ++j){
                errs() << (pointerInfoMapIT->second).pointerAccessPatternConstant(i,j) << " ";
            }
            errs() << "\n";
        }
    }
}
/*-----------------------------DEBUG FUNCTIONS-------------------------------*/