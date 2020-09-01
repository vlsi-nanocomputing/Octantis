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

class FiniteStateMachine
{
public:
    FiniteStateMachine();

    //Function to allocate a new operation in a precise time
    void addNewInstruction(int * time, int ** operand);

private:
    int time;

    //Iterator over the FSM map
    std::map<int, std::list<int *>>::iterator FSMIt;


public:
    std::map<int, std::list<int *>> FSM;
};

}//End Octantis namespace

#endif // FINITESTATEMACHINE_H
