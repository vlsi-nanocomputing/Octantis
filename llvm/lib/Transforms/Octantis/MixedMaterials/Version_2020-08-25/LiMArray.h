/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LimArray Class: it implements the necessary structures to model the LiM Unit
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef LIMARRAY_H
#define LIMARRAY_H

#include <map>
#include <list>
#include <iterator>
#include <string>

namespace octantis{
class LiMArray
{
public:
    //Default constructor
    LiMArray();

    //Function to add a new LiM Row inside the array: LiM row
    void addNewRow(int ** rowName, std::string * rowType, int * rowLength);

    //Function to add a new LiM Row inside the array: LiM row
    void addNewLiMRow(int ** rowName, std::string * rowType, int * rowLength, int ** src);

    //Function to add a new LiM Row inside the array: partial result row
    void addNewResultRow(int ** rowName, int * rowLength, int ** src1);

    //Function to change the type of a LiM row
    bool changeLiMRowType(int **rowName, std::string * newRowType);

    //Function to add a new input connection to a LiM row
    void addNewInputConnection(int **rowName, int **srcRowName);

    //Function to know if a memory row has the same type requested
    bool isLiMRowOfThisType(int **rowName, std::string * type);

    //Function to return the dimensions of the LiM Array
    int getDimensions();


public:

    //Lim row description
    struct LiMRow {
        std::string rowType; //This can be optimized considering enum
        int rowLenght;
        std::list<int *> inputConnections;
        //It can be useful also the output connections: an output mux may be necessary in case
        //of LiM cells! Maybe not: The LiM cell is automatically provided with an output mux!
    };

public:

    //LiM Array
    std::map<int *, LiMRow> limArray;

    //Iterator over LiM Array
    std::map<int *, LiMRow>::iterator limArrayIT;

    //Dimension of the LiM Array
    int numRows;

private:

    //Function to check if a memory row is LiM or not
    bool checkIfRowIsLiM(std::map<int *, LiMRow>::iterator * limArrayItpt);

    //Function to find element inside the limArray
    std::map<int *, LiMRow>::iterator findRow(int *rowName);


};
} //End Octantis Namespace

#endif // LIMARRAY_H
