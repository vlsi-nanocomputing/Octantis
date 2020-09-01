/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LimArray Class: it implements the necessary structures to model the LiM Unit
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "LiMArray.h"

//Octantis Include Files
#include "OperationsImplemented.h"

//LLVM Include Files
#include "llvm/Support/raw_ostream.h"

using namespace octantis;
using namespace llvm;


LiMArray::LiMArray()
{

}

//Function to add a new memory Row inside the array
//      NOTEs: The type is not essential, its default value should be "norm"
void LiMArray::addNewRow(int ** rowName, std::string * rowType, int * rowLength){

    LiMRow tmpRow;
    tmpRow.rowType=*rowType;
    tmpRow.rowLenght=*rowLength;

    limArray.insert({*rowName,tmpRow});

}

//Function to add a new LiM Row inside the array
void LiMArray::addNewLiMRow(int ** rowName, std::string * rowType, int * rowLength, int ** src){

    LiMRow tmpRow;
    tmpRow.rowType=*rowType;
    tmpRow.rowLenght=*rowLength;
    tmpRow.inputConnections.push_back(*src);

    limArray.insert({*rowName,tmpRow});
}

//Function to add a new Row inside the array: partial result row
//      NOTEs: The "norm" type has to be defined somewhere else
void LiMArray::addNewResultRow(int ** rowName, int * rowLength, int ** src){

    LiMRow tmpRow;
    tmpRow.rowType="norm";
    tmpRow.rowLenght=*rowLength;
    tmpRow.inputConnections.push_back(*src);

    limArray.insert({*rowName,tmpRow});

}

//Function to change the type of a LiM row: if a row is just defined as LiM
//it returns "false" and nothing changes.
bool LiMArray::changeLiMRowType(int **rowName, std::string * newRowType){

    //Iterator over LiM Array
    std::map<int *, LiMRow>::iterator limArrayIntIT=findRow(*rowName);

    //Check if the memory row is Lim or not (The iterator is constant)
    if(!checkIfRowIsLiM(&limArrayIntIT)){
        limArrayIntIT->second.rowType=(* newRowType);
        return true;
    } else {
        return false;
    }
}


//Function to check if a memory row is LiM or not
//  NOTEs: this has to be updated to remove the "norm" lable!
bool LiMArray::checkIfRowIsLiM(std::map<int *, LiMArray::LiMRow>::iterator * limArrayItpt){

    //Check if the the Row is type "norm"
    return (((*limArrayItpt)->second.rowType)!= "norm") ? true : false;
}


//Function to find element inside the limArray
std::map<int *, LiMArray::LiMRow>::iterator LiMArray::findRow(int *rowName){

    limArrayIT=limArray.find(rowName);

    if(limArrayIT!=limArray.end())
    {
        //The row is present inside the array
        return limArrayIT;

    } else {
        //An error occurred in finding a specific row
        llvm_unreachable("LiMArray error: Error in finding a row inside the LiMArray.");
    }

}

//Function to add a new input connection to a LiM row if not present
void LiMArray::addNewInputConnection(int **rowName, int **srcRowName){

    //Iterator over LiM Array
    std::map<int *, LiMRow>::iterator limArrayIntIT=findRow(*rowName);

    //Set an alias for the list
    std::list<int *> * connectionsList=&(limArrayIntIT->second.inputConnections);

    //Add the new input connection
    if(find((*connectionsList).begin(), (*connectionsList).end(), *srcRowName) != (*connectionsList).end())
        (*connectionsList).push_back(*srcRowName);

}

//Function to know if a memory row has the same type requested
bool LiMArray::isLiMRowOfThisType(int **rowName, std::string * type){

    //Iterator over LiM Array
    std::map<int *, LiMRow>::iterator limArrayIntIT=findRow(*rowName);

    //Add the new input connection
    return ((limArrayIntIT->second.rowType)==*type);

}

//Function to return the dimensions of the LiM Array
int LiMArray::getDimensions(){
    return limArray.size();
}



