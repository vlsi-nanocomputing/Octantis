/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// AccessPattern Class: Class useful for storing information regarding the access pattern of an array
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef ACCESSPATTERN_H
#define ACCESSPATTERN_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information
class AccessPattern
{

public:

    ///enum type for array access pattern
    enum arrayAccessPattern{
        rowMajor,

        colMajor,

        colMajorSet1ColMajor,
        colMajorSet1RowMajor,
        rowMajorSet1RowMajor,
        rowMajorSet1ColMajor,

        colMajorSet2RowMajor,
        rowMajorSet2ColMajor,
        colMajorSet2ColMajor,
        rowMajorSet2RowMajor,

        undefined,
    };


    ///Default constructor
    AccessPattern(){};

    ///Constructor where rows and colums of the matrix can be specified
    AccessPattern(int rows, int cols)
        :rowsM(rows), colsM(cols){}

    ///Operator used to access an element of the matrix
    int operator () (int i, int j);

    ///Function useful to push an element inside the matrix
    void pushIncrement(int el);

    ///Function returning the matrix
    std::vector<int> getIncrementVector();

    ///Function returning the number of matrix rows
    int getRows();

    ///Function returning the number of matrix columns
    int getCols();

    ///Function useful to set the number of rows of the matrix
    void setRows(int rows);

    ///Function useful to set the number of columns of the matrix
    void setCols(int cols);

    ///Function useful to get the number of initial columns full of zeros
    int getInitZeros();

    ///Function useful to get the number of final columns full of zeros
    int getFinalZeros();

    ///Function useful to set the type of the apm
    void setApmType(arrayAccessPattern apmT);

    ///Function useful to get the type of the apm
    arrayAccessPattern getApmType();

    ///Function that eliminates every column of zeros in the matrix
    void pruneIncrementVector();

    ///It detects the array access pattern
    void detectAccessPatternType();

    ///It detects the columns of zeros in the apm and sets the related internal variables
    void detectZeroCols();

private:

    ///Matrix rows and columns
    int rowsM;
    int colsM;

    ///Access Pattern Matrix
    std::vector<int> incrementVector;

    //Initial columns full of zeros
    int initZerosM;
    //Final columns full of zeros
    int finalZerosM;

    arrayAccessPattern apmType;

};

} //End of Octantis' namespace

#endif // ACCESSPATTERN_H
