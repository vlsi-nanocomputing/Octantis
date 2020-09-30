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

        //Check if the element refers to an iterator (for loops)
        itVariablesMapIT=itVariablesMap.find((int*)I.getOperand(0));
        if(itVariablesMapIT!=itVariablesMap.end())
        {
            //Loop iterator! It has not to be scheduled
            //  NOTE: A loop iterator cannot be considered
            //        for other calculations!! Please update
            //        the behavior of the scheduler or consider
            //        another variable into the input code.

            //Update the alias map
            aliasMap.insert({(int*)&I,(int *) I.getOperand(0)});

        }
        else {

            //Check if the parsed instruction refers to an array
            if (infoAboutPtr.valid==true)
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

            //Check if the instruction is inside a loop
            if(loopInfo.valid==true)
            {
                IT.AddInstructionToList(Timer, Timer, I.getOpcodeName(), (int *) &I, parentReg, nullptr, loopInfo.iterations);
            } else {
                int iterations=1;
                IT.AddInstructionToList(Timer, Timer, I.getOpcodeName(), (int *) &I, parentReg, nullptr,iterations);
            }

            aliasMap.insert(std::pair<int * const, int * const>((int *)&I, parentReg)); //Check the order
        }
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

        } else if(I.getOpcode()==Instruction::Shl || I.getOpcode()==Instruction::LShr || I.getOpcode()==Instruction::AShr){
            //A shift operation has been fetched
            errs()<< "Shift statement detected!\n";

            //Variable to store the control signal
            int * controlSig;

            //Invalidate the signal for the shift (control signal)
            //and introduction of a new control signal inside the proper list
            controlSig=(int *) I.getOperand(1);
            IT.RemoveInstructionFromList(controlSig);
            IT.addControlSignal(controlSig);

            //Add shift operand inside the input row
            IT.AddShiftToList((int *)I.getOperand(0),I.getOpcodeName());

            //Add the destination register to the alias map
            aliasMap.insert(std::pair<int * const, int * const>((int *)&I, (int *)I.getOperand(0)));

        } else {

                //Define the minimum time in which schedule the operation
                //(i.e. when all the operands are ready)
                int Top1=IT.getAvailableTime((int *)I.getOperand(0));
                int Top2=IT.getAvailableTime((int *)I.getOperand(1));

                int Tex=std::max(Top1,Top2);

                //The result will be written at the next clock cycle
                Tex++;

                if(loopInfo.valid==true)
                {
                    IT.AddInstructionToList(Tex, Tex, I.getOpcodeName(), (int *) &I, (int *)I.getOperand(0), (int *)I.getOperand(1),loopInfo.iterations);
                } else {
                    int iterations=1;
                    IT.AddInstructionToList(Tex, Tex, I.getOpcodeName(), (int *) &I, (int *)I.getOperand(0), (int *)I.getOperand(1),iterations);
                }


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

        if(ConstantInt* CI = dyn_cast<ConstantInt>(offset)){
            index=CI->getSExtValue();
        } else {
            //Temporary variable for the parent variable
            //derived from the store instruction
            int * parent=(int *) offset;

            //Find if it represents a constant
            itVariablesMapIT=itVariablesMap.find((int *) offset);

            if(itVariablesMapIT==itVariablesMap.end()){
                parent=getRealParent(parent);
            }

            index=itVariablesMap[parent];
        }



        // Set the useful information for the following input instruction
        infoAboutPtr.ptrName=(int *)&I;
        infoAboutPtr.srcReg=(int *) GEP->getPointerOperand();
        infoAboutPtr.index=index;
        infoAboutPtr.valid=true;
    }
        break;

    case swi:
    {
        //Implementation of multiple logic inside a LiM cell
        errs()<< "Switch statement detected!\n";

        SwitchInst *SWI =dyn_cast<SwitchInst>(&I);
        if(!SWI){
            // The instruction doesn't represent a switch statement
            llvm_unreachable("SchedulingASAP error: current instruction not recognized.\n");
        }

        //Definition of the structure characterizing the switch stat.
        switchStruct intSwitchStruct;

        //Variable for the control signal
        int * controlSig;

        //intSwitchStruct.destReg=(int *) &I; //Not correct: the destination is a register
        intSwitchStruct.numOfCases=SWI->getNumCases();

        errs() << "\tSwitch information:\n";
        errs() <<"\t\t-Num of cases: " << SWI->getNumCases() << "\n";
        errs() <<"\t\t-Num of Operands: " << SWI->getNumOperands() << "\n";

        //The default value is DISCARDED:
        invalidBB.push_back((int *) SWI->getDefaultDest());

        //Invalidate the signal for the mux selection (control signal)
        //and introduction of a new control signal inside the proper list
        controlSig=(int *) SWI->getCondition();
        IT.RemoveInstructionFromList(controlSig);
        IT.addControlSignal(controlSig);


        if(SWI->getNumCases() >= 2)
        {
            //Visiting the switch stamentes
            for (unsigned var = 2, end = SWI->getNumOperands(); var != end; var += 2) {

                //Get the condition
                Value *condition = SWI->getOperand(var);
                ConstantInt* CI = dyn_cast<ConstantInt>(condition);
                errs() << "The condition of the switch statement is: " << CI->getSExtValue() << "\n";

                //Get the associated basic block
                Value *value = SWI->getOperand(var);
                assert(value); //Check if the operations has gone well
                BasicBlock *nextBB = dyn_cast<BasicBlock>(SWI->getOperand(var+1));

                //Invalidate the associated basic block
                invalidBB.push_back((int *) &*nextBB);

                for(Instruction &currentInst : *nextBB)
                {
                    //Check if the intSwitchStruct has invalid members (first case)
                    if(var==2){

                        //Recursive call for the alloca and store operations
                        if(isa <LoadInst> (currentInst) || isa <StoreInst> (currentInst))
                        {
                            addNewInstruction(currentInst);
                        }

                    }

                    //Discarding all the instructions not needed for the definition
                    //of the implemented operations inside the cell
                    if(isa <BinaryOperator> (currentInst))
                    {
                        (intSwitchStruct.operators).push_back(currentInst.getOpcodeName());
                        errs() << "\t\tThe content of the switch for operand "<< value << " is: " << currentInst << "\n";

                        //Check if the intruction belongs to the first case
                        if(intSwitchStruct.valid==false){
                            intSwitchStruct.destReg=(int *) &currentInst;
                            intSwitchStruct.srcReg1=(int *) currentInst.getOperand(0);
                            intSwitchStruct.srcReg2=(int *) currentInst.getOperand(1);
                            intSwitchStruct.valid=true;
                        }
                    }

                }
            }


            //Define the minimum time in which schedule the operation
            //(i.e. when all the operands are ready)
            int Top1=IT.getAvailableTime(intSwitchStruct.srcReg1);
            int Top2=IT.getAvailableTime(intSwitchStruct.srcReg2);

            int Tex=std::max(Top1,Top2);

            //The result will be written at the next clock cycle
            Tex++;


            if(loopInfo.valid==true)
            {
                IT.AddSwitchInstructionToList(Tex, Tex, "switch", intSwitchStruct.operators, intSwitchStruct.destReg,
                                              intSwitchStruct.srcReg1, intSwitchStruct.srcReg2, loopInfo.iterations);
            } else {
                int iterations=1;
                IT.AddSwitchInstructionToList(Tex, Tex, "switch", intSwitchStruct.operators, intSwitchStruct.destReg,
                                              intSwitchStruct.srcReg1, intSwitchStruct.srcReg2, iterations);
            }





        } else {
            //The switch statement has less that two ways, error in the definition of the instruction
            llvm_unreachable("Error in SchedulingASAP: the definition of the switch statement is incorrect.");
        }

    }
        break;

    case ret:
    {
        //To be implemented: actually it doesn't do anything!
    }
        break;

    case branch:
    {
        //To be implemented: actually it doesn't do anything!
    }
        break;

    case sext:
    {
        //Sign extension: not actually relevant.

        //Update of the label associated to the loaded data
        int * parentReg;
        int * newParentReg;
        int * tmp;

        parentReg=(int*) I.getOperand(0);
        newParentReg=(int*) &I;

        aliasMapIT=aliasMap.find(parentReg);
        if(aliasMapIT!=aliasMap.end())
        {
            tmp=aliasMapIT->second;
            aliasMap.erase(aliasMapIT);
            aliasMap.insert({newParentReg,tmp});

        } else {
            llvm_unreachable("Error in SchedulingASAP: in aliasMap looking for a not present varible.");
        }

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

    if(isa<SwitchInst> (I))
        return swi;

    if(isa<BranchInst> (I))
        return branch;

    if(isa<ICmpInst> (I))
        return icmp;

    if(isa<SExtInst> (I))
        return sext;

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

///Function to parse Loops information
void SchedulingASAP::parseLoopInfo(BasicBlock &BB){

    //A loop has been identified
    loopInfo.valid=true;
    loopInfo.loopHeader=(int *) &BB;

    // Parsing the internal instructions
    for (Instruction &I : BB) {

        Instr i=identifyInstr(I);

        errs()<<"Fetched: " << I << "; recognized: " << i << "\n";

        switch(i)
        {
            case load:
            {
                //This is the variable of the counter
                //Update of the associated map to keep track
                //its value.
                //  NOTE: the index strarting from 0 is an
                //        assumption that has to be removed!!
                itVariablesMap.insert({(int*)I.getOperand(0),0});

            }
                break;

            case icmp:
            {
                //Condition through which determine the number
                //loop iterations
                Value *iter = I.getOperand(1);
                ConstantInt* CI = dyn_cast<ConstantInt>(iter);
                int iterations=CI->getSExtValue();

                errs() << "The iterations of the loop statement are: " << iterations << "\n";
                loopInfo.iterations=iterations;

            }
                break;

            case branch:
            {
                //Identification of the body bb and the terminator bb
                loopInfo.loopBody=(int *) I.getOperand(2);
                loopInfo.loopTerm=(int *) I.getOperand(4);

                errs() << "Loop body: " << (int *) I.getOperand(2) << "\n";
                errs() << "Loop terminator: " << (int *) I.getOperand(4) << "\n";

            }
                break;

            default:
            {
                llvm_unreachable("SchedulingASAP error: current instruction inside loop not recognized.\n");
            }

        }

    }
}

///Function to know if a basic block is the last one belonging to a loop
bool SchedulingASAP::isTheLastBBInLoop(BasicBlock &BB){
    //The loop start again during the next iteration
    return (loopInfo.loopHeader==(int *)&BB) ? true : false;
}


/// Fuction useful to define if a basic block is valid (switch
/// cases)
bool SchedulingASAP::isBBValid(BasicBlock &bb){

    invalidBBIT=find(invalidBB.begin(), invalidBB.end(), (int*)&bb);
    return (invalidBBIT==invalidBB.end()) ? true : false;

}

/// Function useful to set a basic block as not valid (loop
/// case)
void SchedulingASAP::setBBAsNotValid(BasicBlock &bb){
     invalidBB.push_back((int *) &bb);
}

///Function to terminate the scheduling of loop instructions
void SchedulingASAP::endOfCurrentLoop(){
    loopInfo.valid=false;
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
