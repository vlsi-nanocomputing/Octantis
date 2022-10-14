/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// FiniteStateMachine Class: useful for the definition of the FSM of the algorithm.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef FINITESTATEMACHINE_H
#define FINITESTATEMACHINE_H

//Standard C++ Include Libraries
#include <map>
#include <list>
#include <iterator>

namespace octantis{

///Class useful for the definition of the FSM of the algorithm.
class FiniteStateMachine
{
public:

    ///Struct for mux selection signals handling
    struct selectionSignalStruct{
        int* destRow;

        std::list<int> activeTimeList;
    };

    ///Default Constructor
    FiniteStateMachine();

    ///Function to allocate a new operand in a precise time in FSM2
    void addNewInstruction(int time, int* const& operand);

    ///Function returning the active time of an operand in FSM2
    int getTime(int* const& operand);

    ///Delete operand from FSM2
    void deleteInstruction(int* const& operand);

    ///Function to get the size of FSM
    int getFSMSize();

    ///It implements the transformation of a FSM2 into FSM 
    void transform();

    ///Inserts a control signal in selectionSignalsList
    void insertSelectionSignal(std::list<int> const &activeTimeList, int* const &destRow);


private:
    int time;

    ///Iterator over the FSM map
    std::map<int, std::list<int *>>::iterator FSMIt;

    ///Iterator over the FSM2 map
    std::map<int*, int>::iterator FSM2It;


public:

    ///Two data structures for FSM are available

    ///The key field refers to the time interval and the value field contains the list of instructions to be executed
    std::map<int, std::list<int *>> FSM;

    ///The key field refers to an instruction and the value field contains the time in which it has to be executed
    std::map<int*, int> FSM2;

    ///list containing selection signals info
    std::list<selectionSignalStruct> selectionSignalsList;

//------------------------DEGUB FUNCTION---------------------
    void printFSM();

    void printSelectionSignals();
//--------------------END DEGUB FUNCTION---------------------

};

}//End Octantis namespace

#endif // FINITESTATEMACHINE_H
