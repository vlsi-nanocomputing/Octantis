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
/*std::pair<PointerInfoTable::arrayAccessPattern, std::vector<int>>  PointerInfoTable::detectPointerAccessPattern(AccessPattern ptrAP){

    PointerInfoTable::arrayAccessPattern tmpPattern = undefined;

    //int iInitPos, jInitPos;
    //bool firstEl = true;
    //std::vector<int> incrementVectorRowMaj, incrementVectorColMaj, incrementVectorRowDiag, incrementVectorColDiag;

    //bool isRowMajor = true;
    //bool isColMajor = true;
    //bool isRowDiag = true;
    //bool isColDiag = true;

    //Getting rows and columns of the AP
    int patternRows = ptrAP.getRows();
    int patternCols = ptrAP.getCols();

    //Initial and final columns of zeros in the AP
    int initZeros = 0;
    int finalZeros = 0;

    bool patternFound = false;

    //If more than 1 patternRow, the AP refers to a matrix
    if(patternRows > 1){

        //Cycling over the AP
        for(int i = 0; i < patternRows && !patternFound; ++i){
            for(int j = 0; j < patternCols && !patternFound; ++j){

                //When an element equal to 1 is found, the position of the other not-null elements in the AP is checked
                if(ptrAP(i,j) == 1){

                    if(ptrAP(i+1,j-1) == 1 && ptrAP(i+1,j+1) == 1 && ptrAP(i,j+2) == 1){

                        //column major with 2-dim sets going as column major
                        initZeros = i - 1;
                        finalZeros = patternCols - i - 3;
                        tmpPattern = colMajorSet2ColMajor;
                        patternFound = true;

                    }else if(ptrAP(i,j+2) == 1 && ptrAP(i+1,j+1) == 1 && ptrAP(i+1,j+3) == 1){

                        //row major with 2-dim sets going as row major
                        initZeros = i;
                        finalZeros = patternCols - i - 4;
                        tmpPattern = rowMajorSet2RowMajor;
                        patternFound = true;

                    }else if(ptrAP(i,j+1) == 1 && ptrAP(i+1,j-1) == 1 && ptrAP(i+1,j+2) == 1){

                        //row major with 2-dim sets going as column major
                        initZeros = i - 1;
                        finalZeros = patternCols - i - 3;
                        tmpPattern = rowMajorSet2ColMajor;
                        patternFound = true;

                    }else if(ptrAP(i,j+3) == 1 && ptrAP(i+1,j+1) == 1 && ptrAP(i+1,j+2) == 1){

                        //column major with 2-dim sets going as row major
                        initZeros = i;
                        finalZeros = patternCols - i - 4;
                        tmpPattern = colMajorSet2RowMajor;  
                        patternFound = true;                      

                    }else if(ptrAP(i,j+2) == 1 && ptrAP(i+1,j+1) == 1){

                        //column major with 1-dim vector sets going as row major
                        initZeros = i;
                        finalZeros = patternCols - i - 3;
                        tmpPattern = colMajorSet1RowMajor;
                        patternFound = true;

                    }else if(ptrAP(i+1,j-1) == 1 && ptrAP(i+1,j+1) == 1){

                        //row major with 1-dim vector sets going as column major
                        initZeros = i - 1;
                        finalZeros = patternCols - i - 2;
                        tmpPattern = rowMajorSet1ColMajor;
                        patternFound = true;

                    }else if(ptrAP(i+1,j+1) == 1){

                        //simple row major
                        initZeros = i;
                        finalZeros = patternCols - i - 2;
                        tmpPattern = rowMajorPosColPos;
                        patternFound = true;

                    }else if(ptrAP(i+1,j-1) == 1){

                        //simple column major
                        initZeros = i - 1;
                        finalZeros = patternCols - i - 1;
                        tmpPattern = colMajorPosRowPos;
                        patternFound = true;

                    }

                }

            }
        }
    }else{
        //If not more than 1 patternRow, the AP refers to a vector

        for(int i = 0; i < patternCols && !patternFound; ++i){

            if(ptrAP(0,i) == 1){
                
                initZeros = i;
                finalZeros = patternCols - i - 1;
                tmpPattern = rowMajorPosColPos;
                patternFound = true;

            }

        }
    }

    //fill info about zeros
    std::vector<int> zerosVect;
    zerosVect.push_back(initZeros);
    zerosVect.push_back(finalZeros);

    //Building the return parameter
    std::pair<PointerInfoTable::arrayAccessPattern, std::vector<int>> retPair;
    retPair.first = tmpPattern;
    retPair.second = zerosVect;

    errs() << "Detected array access pattern: " << tmpPattern <<  " with zeros: " << zerosVect.at(0) << "   " << zerosVect.at(1) << "\n";
    return retPair;

    
}*/

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
        errs()<< "\tpointer: " << pointerInfoMapIT->first << "\n"
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
                << "\t\tnumberOfSubsets: " << (pointerInfoMapIT->second).numberOfSubsets << "\n"
                << "\t\tnumberOfSubsetsElements: " << (pointerInfoMapIT->second).numberOfSubsetsElements << "\n"

                << "\t\tAPM: " << (pointerInfoMapIT->second).pointerAccessPattern.getApmType() << "\n"
 
                << "\t\tfirstIdx: " << (((pointerInfoMapIT->second).firstIdxInfo).iterators).front() << "\n"
                << "\t\tsecondIdx: " << (((pointerInfoMapIT->second).secondIdxInfo).iterators).front() << "\n";

        errs() << "\t\tArray access pattern\n";

        for(int i = 0; i < ((pointerInfoMapIT->second).pointerAccessPattern).getRows(); ++i){
            errs() << "\t\t\t";
            for(int j = 0; j < ((pointerInfoMapIT->second).pointerAccessPattern).getCols(); ++j){
                errs() << (pointerInfoMapIT->second).pointerAccessPattern(i,j) << " ";
            }
            errs() << "\n";
        }


        errs() << "\t\tArray access pattern constant\n";
        
        for(int i = 0; i < ((pointerInfoMapIT->second).pointerAccessPatternConstant).getRows(); ++i){
            errs() << "\t\t\t";
            for(int j = 0; j < ((pointerInfoMapIT->second).pointerAccessPatternConstant).getCols(); ++j){
                errs() << (pointerInfoMapIT->second).pointerAccessPatternConstant(i,j) << " ";
            }
            errs() << "\n";
        }
    }
}
/*-----------------------------DEBUG FUNCTIONS-------------------------------*/