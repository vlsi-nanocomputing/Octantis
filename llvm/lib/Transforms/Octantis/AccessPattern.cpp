/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// AccessPattern Class: Class useful for storing information regarding the access pattern of an array
//                      
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "AccessPattern.h"

//LLVM Include Files
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Operator used to access an element of the matrix
int AccessPattern::operator()(int i, int j){

    return incrementVector[i * colsM + j];

}

///Function useful to push an element inside the matrix
void AccessPattern::pushIncrement(int el){
    incrementVector.push_back(el);
}

///Function returning the matrix
std::vector<int> AccessPattern::getIncrementVector(){
    return incrementVector;
}

///Function returning the number of matrix rows
int AccessPattern::getRows(){
    return rowsM;
}

///Function returning the number of matrix columns
int AccessPattern::getCols(){
    return colsM;
}

///Function useful to set the number of rows of the matrix
void AccessPattern::setRows(int rows){
    rowsM = rows;
}

///Function useful to set the number of columns of the matrix
void AccessPattern::setCols(int cols){
    colsM = cols;
}

///Function useful to get the number of initial columns full of zeros
int AccessPattern::getInitZeros(){
    return initZerosM;
}

///Function useful to set the number of final columns full of zeros
int AccessPattern::getFinalZeros(){
    return finalZerosM;
}

///Function useful to set the type of the apm
void AccessPattern::setApmType(arrayAccessPattern apmT){
    apmType = apmT;
}

///Function useful to get the type of the apm
AccessPattern::arrayAccessPattern AccessPattern::getApmType(){
    return apmType;
}

///Function that eliminates every column of zeros in the matrix
void AccessPattern::pruneIncrementVector(){

    int newCols = colsM;

    if(rowsM == 2){

        std::vector<int> row1Vector(incrementVector.begin(), incrementVector.begin() + colsM);
        std::vector<int> row2Vector(incrementVector.begin() + colsM, incrementVector.end());
        std::vector<int> row1VectorNew;
        std::vector<int> row2VectorNew;

        std::vector<int>::iterator row1VectorIT = row1Vector.begin();
        std::vector<int>::iterator row2VectorIT = row2Vector.begin();
        

        for(int i = 0; i < colsM; ++i){
        
            advance(row1VectorIT, i);
            advance(row2VectorIT, i);

            if(*row1VectorIT != 0 || *row2VectorIT!= 0){

                row1VectorNew.push_back(*row1VectorIT);
                row2VectorNew.push_back(*row2VectorIT);

            }else{

                newCols--;

            }

        }

        row1VectorNew.insert(row1VectorNew.end(), row2VectorNew.begin(), row2VectorNew.end());
        incrementVector = row1VectorNew;

    }else{

        std::vector<int> row1Vector(incrementVector.begin(), incrementVector.end());
        std::vector<int> row1VectorNew;

        std::vector<int>::iterator row1VectorIT = row1Vector.begin();

        int newCols = colsM;

        for(int i = 0; i < colsM; ++i){
        
            advance(row1VectorIT, i);

            if(*row1VectorIT != 0){

                row1VectorNew.push_back(*row1VectorIT);

            }else{

                newCols--;

            }

        }

        incrementVector = row1VectorNew;

    }

    colsM = newCols;

}


///It detects the array access pattern
void  AccessPattern::detectAccessPatternType(){

    AccessPattern::arrayAccessPattern tmpPattern = undefined;

    //Initial and final columns of zeros in the AP
    int initZeros = 0;
    int finalZeros = 0;

    bool patternFound = false;

    AccessPattern tmpAccessPattern = *this;

    tmpAccessPattern.pruneIncrementVector();

    //If more than 1 patternRow, the AP refers to a matrix
    if(tmpAccessPattern.getRows() > 1){

        //Cycling over the AP
        for(int i = 0; i < tmpAccessPattern.getRows() && !patternFound; ++i){
            for(int j = 0; j < tmpAccessPattern.getCols() && !patternFound; ++j){

                //When an element equal to 1 is found, the position of the other not-null elements in the AP is checked
                if(tmpAccessPattern(i,j) != 0){

                    if(tmpAccessPattern.getCols() == 4){

                        if(tmpAccessPattern(i+1,j-1) != 0 && tmpAccessPattern(i+1,j+1) != 0 && tmpAccessPattern(i,j+2) != 0){

                            //column major with 2-dim sets going as column major
                            initZeros = i - 1;
                            finalZeros = tmpAccessPattern.getCols() - i - 3;
                            tmpPattern = colMajorSet2ColMajor;
                            patternFound = true;

                        }else if(tmpAccessPattern(i,j+2) != 0 && tmpAccessPattern(i+1,j+1) != 0 && tmpAccessPattern(i+1,j+3) != 0){

                            //row major with 2-dim sets going as row major
                            initZeros = i;
                            finalZeros = tmpAccessPattern.getCols() - i - 4;
                            tmpPattern = rowMajorSet2RowMajor;
                            patternFound = true;

                        }else if(tmpAccessPattern(i,j+1) != 0 && tmpAccessPattern(i+1,j-1) != 0 && tmpAccessPattern(i+1,j+2) != 0){

                            //row major with 2-dim sets going as column major
                            initZeros = i - 1;
                            finalZeros = tmpAccessPattern.getCols() - i - 3;
                            tmpPattern = rowMajorSet2ColMajor;
                            patternFound = true;

                        }else if(tmpAccessPattern(i,j+3) != 0 && tmpAccessPattern(i+1,j+1) != 0 && tmpAccessPattern(i+1,j+2) != 0){

                            //column major with 2-dim sets going as row major
                            initZeros = i;
                            finalZeros = tmpAccessPattern.getCols() - i - 4;
                            tmpPattern = colMajorSet2RowMajor;  
                            patternFound = true;                      

                        }

                    }else if(tmpAccessPattern.getCols() == 3){
                        
                        if(tmpAccessPattern(i,j+2) != 0 && tmpAccessPattern(i+1,j+1) != 0){

                            //column major with 1-dim vector sets going as row major
                            initZeros = i;
                            finalZeros = tmpAccessPattern.getCols() - i - 3;
                            tmpPattern = colMajorSet1RowMajor;
                            patternFound = true;

                        }else if(tmpAccessPattern(i+1,j-1) != 0 && tmpAccessPattern(i+1,j+1) != 0){

                            //row major with 1-dim vector sets going as column major
                            initZeros = i - 1;
                            finalZeros = tmpAccessPattern.getCols() - i - 2;
                            tmpPattern = rowMajorSet1ColMajor;
                            patternFound = true;

                        }

                    }else if(tmpAccessPattern.getCols() == 2){
                        
                        if(tmpAccessPattern(i+1,j+1) != 0){

                            //simple row major
                            initZeros = i;
                            finalZeros = tmpAccessPattern.getCols() - i - 2;
                            tmpPattern = rowMajor;
                            patternFound = true;

                        }else if(tmpAccessPattern(i+1,j-1) != 0){

                            //simple column major
                            initZeros = i - 1;
                            finalZeros = tmpAccessPattern.getCols() - i - 1;
                            tmpPattern = colMajor;
                            patternFound = true;

                        }

                    }

                }

            }
        }
    }else{
        //If not more than 1 patternRow, the AP refers to a vector

        for(int i = 0; i < tmpAccessPattern.getCols() && !patternFound; ++i){

            if(tmpAccessPattern(0,i) != 0 && tmpAccessPattern(0,i + 1) == 0){
                //Case: x[i]

                initZeros = i;
                finalZeros = tmpAccessPattern.getCols() - i - 1;
                tmpPattern = rowMajor;
                patternFound = true;

            }else if(tmpAccessPattern(0,i) != 0 && tmpAccessPattern(0,i + 1) != 0){
                //Case: x[i + j]

                initZeros = i;
                finalZeros = tmpAccessPattern.getCols() - i - 2;
                tmpPattern = rowMajor; //TO BE CHANGED
                patternFound = true;
            }

        }

    }

    if(patternFound){
        apmType = tmpPattern;
        initZerosM = initZeros;
        finalZerosM = finalZeros;
    }else{
        apmType = AccessPattern::arrayAccessPattern::undefined;
        initZerosM = 0;
        finalZerosM = 0;
    }
    
}


///It detects the columns of zeros in the apm and sets the related internal variables
void AccessPattern::detectZeroCols(){

    int nInitZeroCols = 0;
    int nFinalZeroCols = 0;
    bool end = false;

    AccessPattern tmpAccessPattern = *this;

    if(tmpAccessPattern.getRows() == 1){

        for(int i = 0; i < tmpAccessPattern.getCols() && !end; i++){

            if(tmpAccessPattern(0, i) == 0){
                
                nInitZeroCols++;

            }else{

                for(int j = i + 1; j < tmpAccessPattern.getCols(); j++){
                    if(tmpAccessPattern(0, j) == 0){
                        nFinalZeroCols++;
                    }
                }

                end = true;

            }

        }

    }else if(tmpAccessPattern.getRows() == 2){

        for(int i = 0; i < tmpAccessPattern.getCols() && !end; ++i){

            if(tmpAccessPattern(0, i) == 0 && tmpAccessPattern(1, i) == 0){

                nInitZeroCols++;

            }else{

                for(int j = i + 1; j < tmpAccessPattern.getCols(); ++j){
                    if(tmpAccessPattern(0, j) == 0 && tmpAccessPattern(1, j) == 0){
                        nFinalZeroCols++;
                    }
                }

                end = true;

            }

        }

    }

    initZerosM = nInitZeroCols;
    finalZerosM = nFinalZeroCols;

}