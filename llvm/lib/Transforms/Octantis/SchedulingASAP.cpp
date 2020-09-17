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


///Default constructor
SchedulingASAP::SchedulingASAP()
{

}

///Constructor useful to initialize the Instruction Table
SchedulingASAP::SchedulingASAP(Instruction &I)
{
    addNewInstruction(I);
}

///Function useful to add a new LLVM IR instruction to the Intruction Table
void SchedulingASAP::addNewInstruction(Instruction &I)
{
    Instr i=identifyInstr(I);

    errs()<<"Fetched: " << I << "; recognized: " << i << "\n";

    switch(i)
    {
    case alloc:
    {
        //Number of allocated data (e.g. INT=1, VECTOR=N)
        int arraySize=1;

        errs() << "Alloca detected!\n";
        AllocaInst *a=dyn_cast<AllocaInst>(&I);
        Type *ty=(a->getAllocatedType());

       if(ty->isArrayTy())
       {
           errs()<< "\tArray detected!\n";
           arraySize= (int) ty->getArrayNumElements();
           errs()<< "\tDimension: "<< arraySize << "\n";
       }

        IT.AddAllocaInstructionToList(Timer, (int *) &I, arraySize);
    }
        break;

    case load:
    {
        errs() << "Load detected!\n";

        int * parentReg;
        int index=0; //Default value for traditional load instructions (no array)

        //Check if the parsed instruction refers to an array
        if(infoAboutPtr.valid==true)
        {
            //Check if the assumption is correct
            if(infoAboutPtr.ptrName==(int *) I.getOperand(0))
            {
                errs() << "\tCheck on the index is correct!\n";
                //The element derives from an array
                parentReg=infoAboutPtr.srcReg;
                index=infoAboutPtr.index;
                infoAboutPtr.valid=false;
            } else {
                llvm_unreachable("Error in SchedulingASAP: load instruction after GEP refers to an unknown pointer.");
            }

        } else {
            parentReg = (int *) I.getOperand(0);
        }

        if (!IT.isParentValid(parentReg,index)){
            parentReg = getRealParent(parentReg);
        }

        IT.AddInstructionToList(Timer, Timer, I.getOpcodeName(), (int *) &I, parentReg, nullptr);
        aliasMap.insert(std::pair<int * const, int * const>((int *)&I, parentReg)); //Check the order
    }
        break;

    case store:
    {
        errs() << "Store detected!\n";

        int * destReg;
        int index=0; //Default value for traditional load instructions (no array)

        //Check if the parsed instruction refers to an array
        if(infoAboutPtr.valid==true)
        {
            //Check if the assumption is correct
            if(infoAboutPtr.ptrName==(int *) I.getOperand(0))
            {
                //The element derives from an array
                destReg=infoAboutPtr.srcReg;
                index=infoAboutPtr.index;
                infoAboutPtr.valid=false;
            } else {
                llvm_unreachable("Error in SchedulingASAP: store instruction after GEP refers to an unknown pointer.");
            }

        } else {
            destReg=(int *)I.getOperand(1);
        }


        //Invalidate the data previously declared through 'alloca' statement
        IT.invalidateParent(destReg,index);

        //Update the alias map!!!!

        //Set the relation between the newest data and the invalid one
        aliasMap.insert(std::pair<int * const, int * const>((int *)I.getOperand(1), (int *)I.getOperand(0))); //Check the order
    }
        break;

    case binary:
    {
        errs() << "Binary detected!\n";

        //Check if the input function is a NOT
        if(isThisNot(I)){
            errs() << "\tNOT instruction detected!\n";
            changeParentInNOT((int *)I.getOperand(0),I.getOpcodeName(),(int *) &I);

        } else {

                //Define the minimum time in which schedule the operation
                //(i.e. when all the operands are ready)
                int Top1=IT.getAvailableTime((int *)I.getOperand(0));
                int Top2=IT.getAvailableTime((int *)I.getOperand(1));

                int Tex=std::max(Top1,Top2);

                //The result will be written at the next clock cycle
                Tex++;

                IT.AddInstructionToList(Tex, Tex, I.getOpcodeName(), (int *) &I, (int *)I.getOperand(0), (int *)I.getOperand(1));
        }

    }
        break;

    case ptr:
    {
        errs() << "Pointer detected!\n";

        int index;

        //To identify the kind of pointer: array
        GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I);
        if (!GEP){
           // The instruction doesn't represent a pointer to an array
           llvm_unreachable("SchedulingASAP error: current instruction not recognized.\n");
        }

        //Get the index
        Value *offset = GEP->getOperand(2);
        ConstantInt* CI = dyn_cast<ConstantInt>(offset);

        index=CI->getSExtValue();

        // Set the useful information for the following input instruction
        infoAboutPtr.ptrName=(int *)&I;
        infoAboutPtr.srcReg=(int *) GEP->getPointerOperand();
        infoAboutPtr.index=index;
        infoAboutPtr.valid=true;
    }
        break;

    case ret:
    {
        //To be implemented: actually it doesn't do anything!
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

///Function useful to increment the value of the scheduling timer
void SchedulingASAP::incrementTimer()
{
    Timer+=1;
}

///Function useful to decrement the value of the scheduling timer
void SchedulingASAP::decrementTimer()
{
    Timer-=1;
}

///Function that returns the pointer to the complete Instruction Table
InstructionTable * SchedulingASAP::getInstructionTable()
{
    return (&IT);
}

///Function that return the type of the instruction passed
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

    if(isa <ReturnInst> (I))
        return ret;

    if(isa<GetElementPtrInst>(I))
        return ptr;

    //Not valid instruction
    return unknown;
}

///Function to identify the not instruction
bool SchedulingASAP::isThisNot(Instruction &I){
    std::string logicPort;
    int operand;

    logicPort=I.getOpcodeName();

    //Chech if the second operand is an integer value
    if(ConstantInt* CI=dyn_cast<ConstantInt>(I.getOperand(1))){

        //Converting the operand into an integer value (operand)
        if (CI->getBitWidth() <= 32) {
            operand = CI->getSExtValue();

            if(logicPort=="xor"&&operand==-1)
            {
                errs() << "The logic port is a NOT!\n";
                return true;
            }
          }
    }

    //Case in which the second operand is not a -1
    errs() << "The logic port is not a NOT!\n";
    return false;


}

///Fuction to change the parent type in negative logic and to update the correct destReg
void SchedulingASAP::changeParentInNOT(int* const parentName, std::string operation, int* const newParentName){

    //Get the name of the negative logic corresponding to operation
    std::string negativeOperator=changeLogic.getNegativeLogic(operation);

    errs()<< "The negative operator for " << operation << " is: " <<negativeOperator;

    //Change the operator inside the Instruction Table
    IT.ChangeOperatorAndDestReg(parentName,negativeOperator,newParentName);
}

///Function to get the actual source register from which execute the load
int * SchedulingASAP::getRealParent(int* &aliasParent){
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

///Function to return the pointer to the Instruction Table
InstructionTable & SchedulingASAP::getIT(){
    IT.printIT();
    return IT;
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
