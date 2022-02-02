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

