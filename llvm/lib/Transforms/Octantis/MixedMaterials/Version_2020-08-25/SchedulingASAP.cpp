/*-------------------------------------- The Octantis Project --------------------------------------*/
//
//  Implementation of the ASAP Algorithm for the scheduling of the operations.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "SchedulingASAP.h"

//LLVM Include Files
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/User.h"
#include "llvm/Bitstream/BitCodes.h" //Useful to identify the opcodes

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;


//Default constructor
SchedulingASAP::SchedulingASAP()
{

}

//Constructor useful to initialize the Instruction Table
SchedulingASAP::SchedulingASAP(Instruction &I)
{
    addNewInstruction(I);
}

//Function useful to add a new LLVM IR instruction to the Intruction Table
void SchedulingASAP::addNewInstruction(Instruction &I)
{
    Instr i=identifyInstr(I);

    errs()<<"Fetched: " << I << "; recognized: " << i << "\n";

    switch(i)
    {
    case alloc:
    {
        errs() << "Alloca detected!\n";
        IT.AddAllocaInstructionToList(&Timer, (int *) &I);
    }
        break;

    case load:
    {
        errs() << "Load detected!\n";
        int * parentReg = (int*) I.getOperand(0);
        if (!IT.isParentValid(parentReg)){
            parentReg = getRealParent(parentReg);
        }

        IT.AddInstructionToList(&Timer, &Timer, I.getOpcodeName(), (int *) &I, parentReg, nullptr);
        aliasMap.insert(std::pair<int *,int *>((int *)&I, parentReg)); //Check the order
    }
        break;

    case store:
    {
        errs() << "Store detected!\n";

        //Invalidate the data previously declared through 'alloca' statement
        IT.invalidateParent((int *)I.getOperand(1));

        //Set the relation between the newest data and the invalid one
        aliasMap.insert(std::pair<int *,int *>((int *)I.getOperand(1), (int *)I.getOperand(0))); //Check the order
    }
        break;

    case binary:
    {
        errs() << "Binary detected!\n";

        //Define the minimum time in which schedule the operation
        //(i.e. when all the operands are ready)
        int Top1=IT.getAvailableTime((int *)I.getOperand(0));
        int Top2=IT.getAvailableTime((int *)I.getOperand(1));

        int Tex=std::max(Top1,Top2);

        //The result will be written at the next clock cycle
        Tex++;

        IT.AddInstructionToList(&Tex, &Tex, I.getOpcodeName(), (int *) &I, (int *)I.getOperand(0), (int *)I.getOperand(1));

    }
        break;

    default:
    {
        IT.printIT();
        IT.printAllocData();
        printAliasMap();
        llvm_unreachable("SchedulingASAP error: current instruction not recognized.\n");
    }

    }
}

//Function useful to increment the value of the scheduling timer
void SchedulingASAP::incrementTimer()
{
    Timer+=1;
}

//Function useful to decrement the value of the scheduling timer
void SchedulingASAP::decrementTimer()
{
    Timer-=1;
}

//Function that returns the pointer to the complete Instruction Table
InstructionTable * SchedulingASAP::getInstructionTable()
{
    return (&IT);
}

//Function that return the type of the instruction passed
SchedulingASAP::Instr SchedulingASAP::identifyInstr(Instruction &I){

    //Identification of the kind of instruction (see Instruction.def)
    //The instruction set is limited considering that the LLVM IR has been
    //previously optimized by the Transform Passes.
    if(isa<AllocaInst>(I))
        return alloc;

    if(isa <LoadInst> (I))
        return load;

    if(isa <StoreInst> (I))
        return store;

    if(isa <BinaryOperator> (I))
        return binary;

    //Not valid instruction
    return unknown;
}

//Function to get the actual source register from which execute the load
int * SchedulingASAP::getRealParent(int * aliasParent){
    aliasMapIT = aliasMap.find(aliasParent);
    if (aliasMapIT!=aliasMap.end())
    {
        return aliasMap[aliasParent];
    }
    else
    {
        llvm_unreachable("SchedulingASAP error: current instruction refers to a source register not present"
                         " inside the alias map.");
    }
}

/*-----------------------------DEBUG FUNCTIONS-------------------------------*/

void SchedulingASAP::printAliasMap(){

    int lineCount=0;

    errs()<< "The state of the Alias Map will be printed:\n\n";

    for (aliasMapIT = aliasMap.begin(); aliasMapIT != aliasMap.end(); ++aliasMapIT)
    {
        errs()<< "\t\t Line " << lineCount << ": ";
        errs()<< aliasMapIT->first << " : ";
        errs()<< aliasMapIT->second << "\n";

        lineCount++;
    }

    errs()<< "\n\n";

}
/*--------------------------END DEBUG FUNCTIONS------------------------------*/
