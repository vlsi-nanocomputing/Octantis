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
    FiniteStateMachine();

    ///Function to allocate a new operation in a precise time
    void addNewInstruction(int &time, int* const& operand);

    ///Function to get the size of the FSM
    int getFSMSize();


private:
    int time;

    ///Iterator over the FSM map
    std::map<int, std::list<int *>>::iterator FSMIt;


public:
    std::map<int, std::list<int *>> FSM;

//------------------------DEGUB FUNCTION---------------------
    void printFSM();
//--------------------END DEGUB FUNCTION---------------------

};

}//End Octantis namespace

#endif // FINITESTATEMACHINE_H
