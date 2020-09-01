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

#include "llvm/Support/raw_ostream.h"

using namespace octantis;
using namespace llvm;

FiniteStateMachine::FiniteStateMachine()
{
    time=0;
}

///Function to allocate a new operation in a precise time
void FiniteStateMachine::addNewInstruction(int &time, int* const& operand){

    FSMIt=FSM.find(time);

    if(FSMIt!=FSM.end())
    {
        FSMIt->second.push_back(operand);

    } else {
        std::list<int *> listTmp;
        listTmp.push_back(operand);
        FSM.insert({time, listTmp});
    }

}

///Function to get the size of the FSM
int FiniteStateMachine::getFSMSize(){
    return FSM.size();
}

//------------------------DEGUB FUNCTION---------------------
void FiniteStateMachine::printFSM(){

    for(FSMIt=FSM.begin();FSMIt!=FSM.end();++FSMIt){

        errs() << "\tTime t=" << FSMIt->first << "; Active rows: ";
        for(std::list<int *>::iterator listIT=(FSMIt->second).begin();listIT!=(FSMIt->second).end();++listIT)
        {
            errs() << (*listIT) << ", ";
        }
        errs() << "\n";
    }

}
//--------------------END DEGUB FUNCTION---------------------
