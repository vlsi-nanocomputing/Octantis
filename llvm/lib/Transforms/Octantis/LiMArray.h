/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LimArray Class: it implements the necessary structures to model the LiM Unit
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
// © Alessio Nicola 2021 (alessio.nicola@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef LIMARRAY_H
#define LIMARRAY_H

#include <map>
#include <list>
#include <iterator>
#include <string>

namespace octantis{

///Class implementing the necessary structures to model the LiM Unit
class LiMArray
{
public:
    ///Default constructor
    LiMArray();

    ///Function to add a new memory Row inside the array
    ///      NOTEs: The type is not essential, its default value should be "load"
    void addNewRow(int* const & rowName, std::string &rowType, int &rowLength);

    ///Function to add a new LiM Row inside the array: LiM row
    void addNewLiMRow(int* const &rowName, std::string &rowType, std::list<std::string> &addLogic, int &rowLength, int* const &src, std::string const &conType);

    ///Function to add a new LiM Row inside the array: partial result row
    void addNewResultRow(int* const &rowName, int &rowLength, int* const &src1, std::string const &conType);

    ///Function to change the type of a LiM row
    bool changeLiMRowType(int* const &rowName, std::string &newRowType, std::list<std::string> &additionalOperators);

    ///Function to add a new input connection to a LiM row
    void addNewInputConnection(int* const &rowName, int* const &srcRowName, std::string const &conType);

    ///Function to add logic inside a specific LiM row
    void addLogicToRow(int * const &rowName, std::string &additionalLogicType);

    ///Function to know if a memory row has the same type requested
    bool isLiMRowOfThisType(int* const &rowName, std::string &type);

    void deleteLiMRow(int* const &rowName);

    ///Function to return the dimensions of the LiM Array
    int getDimensions();



public:

    ///Lim row description
    struct LiMRow {
        std::string rowType; //This can be optimized considering enum
        int rowLength;
        std::list<int *> inputConnections;
        std::list<std::string> inputConnectionsType;
        std::list<std::string> additionalLogic;

        //It can be useful also the output connections: an output mux may be necessary in case
        //of LiM cells! Maybe not: The LiM cell is automatically provided with an output mux!
    };

public:

    ///LiM Array
    std::map<int * const, LiMRow> limArray;

    ///Iterator over LiM Array
    std::map<int * const, LiMRow>::iterator limArrayIT;

    ///Dimension of the LiM Array
    int numRows;    

    int nOutputs;
private:

    ///Function to check if a memory row is LiM or not
    bool checkIfRowIsLiM(std::map<int * const, LiMRow>::iterator &limArrayItpt);

public:

    ///Function to find element inside the limArray
    std::map<int * const, LiMRow>::iterator findRow(int* const &rowName);

//----------------------------DEBUG FUNCTIONS---------------------------
public:
    void printLiMArray();

//------------------------END DEBUG FUNCTIONS---------------------------


};
} //End Octantis Namespace

#endif // LIMARRAY_H
