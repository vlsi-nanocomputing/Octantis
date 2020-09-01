/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// FiniteStateMachine Class: useful for the definition of the FSM of the algorithm.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "FiniteStateMachine.h"

using namespace octantis;

FiniteStateMachine::FiniteStateMachine()
{
    time=0;
}

//Function to allocate a new operation in a precise time
void FiniteStateMachine::addNewInstruction(int * time, int ** operand){

    FSMIt=FSM.find(*time);

    if(FSMIt!=FSM.end())
    {
        FSMIt->second.push_back(*operand);

    } else {
        std::list<int *> listTmp;
        listTmp.push_back(*operand);
        FSM.insert({*time, listTmp});
    }

}
