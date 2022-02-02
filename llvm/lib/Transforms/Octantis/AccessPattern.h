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

private:

    ///Matrix rows and columns
    int rowsM;
    int colsM;

    ///Access Pattern Matrix
    std::vector<int> incrementVector;

};

} //End of Octantis' namespace

#endif // ACCESSPATTERN_H
