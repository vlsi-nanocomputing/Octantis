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

///Default Constructor
FiniteStateMachine::FiniteStateMachine()
{
    time=0;
}

///Function to allocate a new operand in a precise time
void FiniteStateMachine::addNewInstruction(int &time, int* const& operand){

    FSM2.insert(std::pair<int*, int>(operand, time));

}

///It implements the transformation of a FSM2 into FSM 
void FiniteStateMachine::transform(){

    for(FSM2It = FSM2.begin(); FSM2It != FSM2.end(); ++ FSM2It){

        FSMIt = FSM.find(FSM2It->second);

        if(FSMIt == FSM.end()){

            std::list<int*> tmpEmptyList;
            tmpEmptyList.push_back(FSM2It->first);
            FSM.insert(std::pair<int, std::list<int*>> (FSM2It->second, tmpEmptyList));

        }else{

            (FSMIt->second).push_back(FSM2It->first);

        }
    }
}

///Inserts a control signal in selectionSignalsList
void FiniteStateMachine::insertSelectionSignal(std::list<int> const &activeTimeList, int* const &destRow){

    FiniteStateMachine::selectionSignalStruct tmpControlDataStruct;

    tmpControlDataStruct.destRow = destRow;
    tmpControlDataStruct.activeTimeList = activeTimeList;

    selectionSignalsList.push_back(tmpControlDataStruct);

}

///Function to get the size of the FSM
int FiniteStateMachine::getFSMSize(){
    return FSM.size();
}

///Delete operand from FSM
void FiniteStateMachine::deleteInstruction(int* const& operand){

    std::list<int*>::iterator deleteElIT;

    FSM2.erase(FSM2.find(operand));

}

///Function returning the active time of an operand in FSM2
int FiniteStateMachine::getTime(int* const& operand){

    return (FSM2.find(operand))->second;
    
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

void FiniteStateMachine::printSelectionSignals(){
    
    for(auto listIT = selectionSignalsList.begin(); listIT != selectionSignalsList.end(); ++listIT){
        
        errs() << "2-to-1 muxes inserted in row " << listIT->destRow << " active at times "
                 << (listIT->activeTimeList).front() << " and " << (listIT->activeTimeList).back() << "\n";

    }

}
//--------------------END DEGUB FUNCTION---------------------
