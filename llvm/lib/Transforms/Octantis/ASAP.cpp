/*-------------------------------------- The Octantis Project --------------------------------------*/
//
//                      
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "ASAP.h"

//LLVM Include Files
#include "llvm/IR/Function.h"

#include "CollectInfo.h"
#include "InstructionMap.h"
#include "AdditionalLogicPorts.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Constructor needing a CollectInfo object
ASAP::ASAP(CollectInfo CI)
{
    accumulationValidityCount = 0;
    infoCollection = CI;
}

///Function useful to retrieve the right src register name (which has to correspond to the dest reg name of a previous instruction)
///to assign to the temp src variable in scheduling function
int* ASAP::getRealSrcReg(int * allocatedReg){

    lastUsedRegMapIT = lastUsedRegMap.find(allocatedReg);

    //Choosing the last used register related to the allocated register
    //in order to keep the proper dependecy relationships
    if(lastUsedRegMapIT != lastUsedRegMap.end()){

        return lastUsedRegMapIT->second;

    }else{

        //If not used before, return the allocated reg name
        return allocatedReg;

    }

}

///It handles the insertion in IM of instructions that are not load instructions
void ASAP::handleInstructionInserting(const Instruction &I){
    //Check on the operation name
    if(operation != "null" && operation != "load"){

        //destReg is assigned the result operand of instruction
        destReg = (int*)(&I);

        //If loadCount is greater than zero, hence at least one load has preceeded the store instruction
        if(loadCount > 0){

            //If at this point a src reg is nullptr, it means that a multioperand instruction has been
            //subdivided into 2-srcOperands instructions, hence that tmp variable has not a related load instruction
            //but can be found in the operand zero of the operation instruction
            if(srcReg1 == nullptr){
                srcReg1 = (int*)(I.getOperand(0));
            }

            if(srcReg2 == nullptr){
                srcReg2 = (int*)(I.getOperand(0));
            }


            //Get time init time for this instruction
            ti = getTime(isSrc1Constant, isSrc2Constant, srcReg1, srcReg2);

            //Delay of operation is set to 1
            di = 1;
            
            //checking if instruction is in a loop
            bool isInLoopBody = infoCollection.isLoopBody(I.getParent());

            //inserting instruction in IM
            IM.insertInstruction(ti, di, operation, destReg, tmpPtrDest, srcReg1, tmpPtrSrc1, srcReg2, tmpPtrSrc2, isInLoopBody);


            /////DEBUG/////
            errs() << "Instruction inserted in instructionMap with the following specs:\n";
            errs() << "\t operation: " << operation;
            errs() << "\t ti: " << ti;
            errs() << "\t di: " << di;
            errs() << "\t destReg: " << destReg;
            errs() << "\t ptrDestReg: " << tmpPtrDest;
            errs() << "\t srcReg1: " << srcReg1;
            errs() << "\t ptrSrcReg1: " << tmpPtrSrc1;
            errs() << "\t srcReg2: " << srcReg2;
            errs() << "\t ptrSrcReg2: " << tmpPtrSrc2;
            errs() << "\n\n";
            /////DEBUG/////

        }

        //Resetting almost all tmp variables to nullptr
        //destReg and operation are not reinitialized because they will be useful when a store operation in detected

        //accumulationValidityCount = 0;
        loadCount = 0;

        srcReg1 = nullptr;
        srcReg2 = nullptr;
        //destReg = nullptr;
        tmpPtrSrc1 = nullptr;
        tmpPtrSrc2 = nullptr;
        tmpPtrDest = nullptr;
        //operation = "null";

    } 
}

///It handles store instructions. It means that it updates lastUsedRegMap, detects if dest reg is an array and update infos related to it (tmpDestPtr)
void ASAP::handleStoreInstruction(const Instruction &I){

    //Check on operation name to be not null
    if(operation != "null"){

        //Getting the store destination
        int * storeDest = (int*)(I.getOperand(1));

        //Getting the allocated register related to the store destination register
        int * allocatedReg = infoCollection.getAllocatedReg(storeDest);

        //If the allocated reg is an array
        if(infoCollection.isArray(allocatedReg)){

            //storeDest is a pointer
            tmpPtrDest = storeDest;

            //Check if allocated register is already is lastUsedRegMap
            lastUsedRegMapIT = lastUsedRegMap.find(allocatedReg);

            //If it is not present, insert it with operand 0, which will become the last used reg associated to the allocatedReg
            if(lastUsedRegMapIT == lastUsedRegMap.end()){

                lastUsedRegMap.insert(std::pair<int*, int*>(allocatedReg, (int*)(I.getOperand(0))));

            }else{

                //If it is present, insert the last used reg, that is operand 0
                lastUsedRegMapIT->second = (int*)(I.getOperand(0));

            }

        }else{
            //If the allocated reg is not an array

            //Check if store destination (which corresponds to the allocated reg for a variable) is already is lastUsedRegMap
            lastUsedRegMapIT = lastUsedRegMap.find(storeDest);

            //If it is not present, insert it with operand 0, which will become the last used reg associated to the allocatedReg
            if(lastUsedRegMapIT == lastUsedRegMap.end()){

                lastUsedRegMap.insert(std::pair<int*, int*>(storeDest, (int*)(I.getOperand(0))));

            }else{

                //If it is present, insert the last used reg, that is operand 0
                lastUsedRegMapIT->second = (int*)(I.getOperand(0));

            }

        }

        //If a valid load has been found before a store without an operation in between
        if(loadBeforeStore == 1 && operation == "load"){

            //Handling assigments like a = b or Matrix[i][j] = s
            if(!(dyn_cast<ConstantInt>(I.getOperand(0)))){

                //if the assigned value is not a constant,
                //the destination reg of the instruction whose dest reg was the current src1 is assigned to the current dest reg

                IM.instructionMapIT = (IM.instructionMap).find(srcReg1);
                ((IM.instructionMapIT)->second).destinationReg = destReg;
                ((IM.instructionMapIT)->second).destRegPrt = tmpPtrDest;

                errs() << "Replaced instruction with destReg " << (IM.instructionMapIT->second).destinationReg 
                << " with destReg " << destReg << "\n";

            }                            

        //If no load before a store are present, a constant is likely to be store
        }else if(loadBeforeStore == 0){

            //Handling constant assignments like a = 3
            if(dyn_cast<ConstantInt>(I.getOperand(0))){
                /*
                    TO BE HANDLED
                */
            }    

        //If more than 1 load is present before a store, the operation has been already inserted
        //but eventual specifications or dest reg related pointer are yet to be inserted 
        }else if (loadBeforeStore > 1){

            //Accumulation detection
            if(accumulationValidityCount == 2 && tmpAccumulationOp == (int*)(I.getOperand(1))){

                IM.instructionMapIT = (IM.instructionMap).find(destReg);
                if(((IM.instructionMapIT)->second).isInLoopBody){

                    errs() << "inserting acc spec\n";
                    IM.addSpecToInstruction(destReg, "accumulation");

                }       

            //Inserting dest reg pointer         
            }else{
                IM.insertDestPtr(destReg, tmpPtrDest);
            }
            
        }

        //Resetting tmp variables
        loadBeforeStore = 0;
        accumulationValidityCount = 0;
        loadCount = 0;

        srcReg1 = nullptr;
        srcReg2 = nullptr;
        destReg = nullptr;
        tmpPtrSrc1 = nullptr;
        tmpPtrSrc2 = nullptr;
        tmpPtrDest = nullptr;
        operation = "null";
    }


}

void ASAP::scheduleFunction(std::list<BasicBlock*> basicBlocksToSchedule){

    //Initializing all temporary variable to nullptr or zero
    
    //src and dest operands
    srcReg1 = nullptr;
    srcReg2 = nullptr;
    destReg = nullptr;

    //ptr associated to an operand
    tmpPtrSrc1 = nullptr;
    tmpPtrSrc2 = nullptr;
    tmpPtrDest = nullptr;

    //init time for src1 and src2 operands
    tSrcReg1 = 0;
    tSrcReg2 = 0;

    //temp variable for counting how many load operations have been performed before a store
    loadCount = 0;
    loadBeforeStore = 0;

    //init time of an operation and its "delay"
    ti = 0;
    di = 0;

    //operation string
    operation = "null";

    isSrc1Constant = false;
    isSrc2Constant = false;

    errs() << "Begin scheduling instructions\n";

    //Cycling over all basic block that had been previouly recognized as useful for scheduling
    for(auto listBBIT = basicBlocksToSchedule.begin(); listBBIT != basicBlocksToSchedule.end(); ++listBBIT){

        //Getting current instruction
        for(Instruction &I : *(*listBBIT)){

            errs() << "Identified instruction: " << I << "\n";

            //Checking if its a valid instruction to schedule, i.e. load, store, binary operator
            if(infoCollection.isValidInst(I)){

                errs() << "Instruction is valid!\n";

                //If the instruction is a binaryOperator
                if(dyn_cast<BinaryOperator>(&I)){

                    if(dyn_cast<ConstantInt>(I.getOperand(0))){

                        isSrc1Constant = true;

                    }
                    
                    /*
                        INSERT NEW LOAD INSTRUCTION FOR CONSTANT
                    */
                    if(dyn_cast<ConstantInt>(I.getOperand(1))){

                        isSrc2Constant = true;
                    }


                    //Checking opcode in order to recognize instruction
                    if(I.getOpcode() == Instruction::Add){
                        
                        //setting operation string
                        operation = "add";

                        //update accumulation info
                        if(accumulationValidityCount == 1){
                            accumulationValidityCount++;
                        }

                    }else if(I.getOpcode() == Instruction::SDiv){

                        //setting operation string
                        operation = "sdiv";

                    }else if(I.getOpcode() == Instruction::Mul){
                        //setting operation string
                        operation = "mul";

                    }else if(I.getOpcode() == Instruction::Shl){
                        //setting operation string
                        operation = "shl";

                    }else if(I.getOpcode() == Instruction::LShr){
                        //setting operation string
                        operation = "lshr";

                    }else if(I.getOpcode() == Instruction::AShr){
                        //setting operation string
                        operation = "ashr";

                    }else if(I.getOpcode() == Instruction::Sub){
                        //setting operation string
                        operation = "sub";

                    }else if(I.getOpcode() == Instruction::Xor){
                        operation = "xor";

                        //Detection of XNOR operation
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(I.getOperand(1))){
                            if(tmpLogicOperatorDestReg == (int*)(I.getOperand(0)) && CI->getSExtValue() == 1){
                                operation = changeLogic.getNegativeLogic(operation);
                            }
                        }

                        tmpLogicOperatorDestReg = (int*)(&I);

                    }else if(I.getOpcode() == Instruction::And){
                        operation = "and";

                        //Detection of NAND operation
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(I.getOperand(1))){
                            if(tmpLogicOperatorDestReg == (int*)(I.getOperand(0)) && CI->getSExtValue() == 1){
                                operation = changeLogic.getNegativeLogic(operation);
                            }
                        }

                        tmpLogicOperatorDestReg = (int*)(&I);
                        
                    }else if(I.getOpcode() == Instruction::Or){
                        operation = "or";

                        //Detection of NOR operation
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(I.getOperand(1))){
                            if(tmpLogicOperatorDestReg == (int*)(I.getOperand(0)) && CI->getSExtValue() == 1){
                                operation = changeLogic.getNegativeLogic(operation);
                            }
                        }

                        tmpLogicOperatorDestReg = (int*)(&I);
                        
                    }

                    //Managing the insertion of a new instruction that is not a load
                    handleInstructionInserting(I);

                }else if(dyn_cast<LoadInst>(&I)){

                    operation = "load";

                    //Initializing accumulation temp variables
                    tmpAccumulationOp = (int*)(I.getOperand(0));
                    accumulationValidityCount++;

                    //Getting the allocated register related to the loaded operand
                    int* allocatedReg = infoCollection.getAllocatedReg((int*)(I.getOperand(0)));

                    //If the allocated register refers to an array
                    if(infoCollection.isArray(allocatedReg)){

                        //Check if the register has already been used as dest reg,
                        //case in which a load is not necessary
                        lastUsedRegMapIT = lastUsedRegMap.find((int*)(I.getOperand(0)));
                        if(lastUsedRegMapIT == lastUsedRegMap.end()){
                            //Insert a new load instruction in instructionMap
                            IM.insertLoadInstruction(allocatedReg, infoCollection);
                        }


                        if(loadCount == 0){
                            //Temp pointer is assigned the loaded operand, which is a pointer in case the allocated reg is an array
                            tmpPtrSrc1 = (int*)(I.getOperand(0));
                            //Getting the real scr register
                            srcReg1 = getRealSrcReg(allocatedReg);
                            //incrementing loadcount
                            loadCount++;
                        }else if(loadCount == 1){
                            //Temp pointer is assigned the loaded operand, which is a pointer in case the allocated reg is an array
                            tmpPtrSrc2 = (int*)(I.getOperand(0));
                            //Getting the real scr register
                            srcReg2 = getRealSrcReg(allocatedReg);
                            //incrementing loadcount
                            loadCount++;
                        }

                    }else{

                        //Check if the register has already been used as dest reg,
                        //case in which a load is not necessary
                        lastUsedRegMapIT = lastUsedRegMap.find((int*)(I.getOperand(0)));
                        if(lastUsedRegMapIT == lastUsedRegMap.end()){
                            //Insert a new load instruction in instructionMap
                            IM.insertLoadInstruction((int*)(I.getOperand(0)), infoCollection);
                        }                        

                        //In case the allocated register is a single variable, no pointers are needed
                        //only retrieving the real src reg and assign it to the proper src operand based on loadcount
                        if(loadCount == 0){
                            srcReg1 = getRealSrcReg((int*)(I.getOperand(0)));
                            loadCount++;
                        }else if(loadCount == 1){
                            srcReg2 = getRealSrcReg((int*)(I.getOperand(0)));
                            loadCount++;
                        }

                    }

                    loadBeforeStore++;

                }else if(dyn_cast<StoreInst>(&I)){
                    //The store operation is the one that concludes a load-op-store sequence,
                    //hence, info about lastUsedRegMap are updated here, also checking if the dedsstReg is an array,
                    //case in which info about its pointer must be inserted in IM

                    handleStoreInstruction(I);

                }
            }            
        }
    }

    //reorder the internal instruction vector to IM, preparing for binding phase
    IM.reorderInstructionVector();

}

///Function useful to get the init time for a newly scheduled instruction
int ASAP::getTime(const bool &isSrc1Const, const bool &isSrc2Const, int* &src1, int* &src2){

    int tSrc1, tSrc2;

    //If operand is constant, its init time is 0, else it must be retrieved from IM

    if(isSrc1Const){
        tSrc1 = 0;
    }else{
        tSrc1 = IM.getAvailableTime(src1);
    }

    if(isSrc2Const){
        tSrc2 = 0;
    }else{
        tSrc2 = IM.getAvailableTime(src2);
    }

    //init time for the new instruction is the max between tSrc1 and tSrc2
    int ti = (tSrc1 > tSrc2) ? tSrc1 : tSrc2;

    return ti;
}

///Function useful to get the entire instructionMap built by the scheduling operation
InstructionMap ASAP::getInstructionMap(){
    return IM;
}