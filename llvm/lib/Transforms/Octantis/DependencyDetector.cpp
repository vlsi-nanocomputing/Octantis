/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// DependencyDetector class: Class useful for the detection of dependencies among schedulable instructions                     
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "DependencyDetector.h"

//LLVM Include Files
#include "llvm/IR/Function.h"

#include "CollectInfo.h"
#include "InstructionTable.h"
#include "AdditionalLogicPorts.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;


void DependencyDetector::setRegType(bool isArray, int * reg, std::string operandType){

    if(operandType == "destRegType"){

        if(isArray){

            IM.modifyGivenField(reg, operandType, nullptr, InstructionTable::array);            
        
        }else{
                
            IM.modifyGivenField(reg, operandType, nullptr, InstructionTable::singleVariable);

        }

    }else{

        if(operandType == "srcReg1Type"){
            if(isArray){

                srcReg1Type = InstructionTable::array;           
                
            }else{              
                    
                srcReg1Type = InstructionTable::singleVariable;    

            }
        }else{
            if(isArray){

                srcReg2Type = InstructionTable::array;           
                
            }else{
               
                srcReg2Type = InstructionTable::singleVariable;       

            }

        }        

    }

}

///Function useful to retrieve the right src register name (which has to correspond to the dest reg name of a previous instruction)
///to assign to the temp src variable in scheduling function
int* DependencyDetector::getRealSrcReg(int * allocatedReg){

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
void DependencyDetector::handleInstructionInserting(const Instruction &I){

    //Check on the operation name
    if(operation != "null" && operation != "load"){

        if((int*)(I.getOperand(0)) == destReg || (int*)(I.getOperand(1)) == destReg){

            if((int*)(I.getOperand(0)) == destReg){
                //Multi-operand

                srcReg2 = (int*)(I.getOperand(0));
                srcReg2Type = destRegType;
                tmpPtrSrc2 = tmpPtrDest;

                if(srcReg1 == nullptr){

                    srcReg1 = (int*)(I.getOperand(1));
                    srcReg1Type = InstructionTable::constant;

                }else{

                    setRegType(isSrc1Array, srcReg1, "srcReg1Type");

                }


            }else if((int*)(I.getOperand(1)) == destReg){
                //Multi-operand
                
                srcReg2 = (int*)(I.getOperand(1));
                srcReg2Type = destRegType;
                tmpPtrSrc2 = tmpPtrDest;

                if(srcReg1 == nullptr){

                    srcReg1 = (int*)(I.getOperand(0));
                    srcReg1Type = InstructionTable::constant;

                }else{

                    setRegType(isSrc1Array, srcReg1, "srcReg1Type");

                }
            } 

                
            if(tmpPtrSrc1 != nullptr || tmpPtrSrc2 != nullptr){

                int* newPtr;
                newPtr = new int;
                PointerInfoTable::pointerInfoStruct newPis;
                PointerInfoTable::pointerInfoStruct pisSrc1;
                PointerInfoTable::pointerInfoStruct pisSrc2;

                if((srcReg1Type == InstructionTable::array || srcReg1Type == InstructionTable::fakeArray) && (srcReg2Type == InstructionTable::singleVariable || srcReg2Type == InstructionTable::constant)){

                    errs() << "1\n\n" "\n\n";

                    pisSrc1 = (infoCollection.PIT).getPointerInfo(tmpPtrSrc1);

                    newPis.numberOfSubsets = pisSrc1.numberOfSubsets;
                    
                    (infoCollection.PIT).insertPointerInfo(newPtr, newPis);

                }else if((srcReg1Type == InstructionTable::singleVariable || srcReg1Type == InstructionTable::constant) && (srcReg2Type == InstructionTable::array || srcReg2Type == InstructionTable::fakeArray)){

                    errs() << "2\n\n" "\n\n";

                    pisSrc2 = (infoCollection.PIT).getPointerInfo(tmpPtrSrc2);

                    newPis.numberOfSubsets = pisSrc2.numberOfSubsets;
                    
                    (infoCollection.PIT).insertPointerInfo(newPtr, newPis);

                }else if ((srcReg1Type == InstructionTable::array || srcReg1Type == InstructionTable::fakeArray) && (srcReg2Type == InstructionTable::array || srcReg2Type == InstructionTable::fakeArray)){

                    errs() << "3\n\n" "\n\n";
                    
                    pisSrc1 = (infoCollection.PIT).getPointerInfo(tmpPtrSrc1);

                    pisSrc2 = (infoCollection.PIT).getPointerInfo(tmpPtrSrc2);

                    newPis.numberOfSubsets = (pisSrc2.numberOfSubsets > pisSrc1.numberOfSubsets) ? pisSrc2.numberOfSubsets : pisSrc1.numberOfSubsets;
                    
                    (infoCollection.PIT).insertPointerInfo(newPtr, newPis);

                }

                errs() << "NewPtr: " << newPtr << "\n\n";

                tmpPtrDest = newPtr;
            
            }           
            
        }else{
            //Two-operand
            
            if(dyn_cast<ConstantInt>(I.getOperand(0))){
                srcReg1Type = InstructionTable::constant;
                srcReg1 = (int*)(I.getOperand(0));
            }else{
                setRegType(isSrc1Array, srcReg1, "srcReg1Type");
            }

            if(dyn_cast<ConstantInt>(I.getOperand(1))){
                srcReg2Type = InstructionTable::constant;
                srcReg2 = (int*)(I.getOperand(1));
            }else{
                setRegType(isSrc2Array, srcReg2, "srcReg2Type");
            }

        }

        destRegType = (srcReg1Type == InstructionTable::array || srcReg2Type == InstructionTable::array ||
                       srcReg1Type == InstructionTable::fakeArray || srcReg2Type == InstructionTable::fakeArray) ? 
                        InstructionTable::fakeArray : 
                        InstructionTable::singleVariable;

        //destReg is assigned the result operand of instruction
        destReg = (int*)(&I); 

        if(debugMode){
            errs() << "srcReg1: " << srcReg1 << ", srcReg2: " << srcReg2 << ", destReg: " << destReg << "\n";
            errs() << "isSrc1Array: " << isSrc1Array << ", isSrc2Array: " << isSrc2Array << ", loadCount: " << loadCount << "\n\n";
        }          



        //checking if instruction is in a loop
        bool isInLoopBody = infoCollection.isLoopBody(I.getParent());

        //inserting instruction in IM
        IM.insertInstruction(0, 0, operation, destReg, destRegType, tmpPtrDest, srcReg1, srcReg1Type, tmpPtrSrc1, srcReg2, srcReg2Type, tmpPtrSrc2, isInLoopBody);


        if(debugMode){
        errs() << "Instruction (temp) inserted in instructionMap with the following specs:\n";
        errs() << "\t operation: " << operation;
        errs() << "\t destReg: " << destReg;
        errs() << "\t destRegType: " << destRegType;
        errs() << "\t ptrDestReg: " << tmpPtrDest;
        errs() << "\t srcReg1: " << srcReg1;
        errs() << "\t srcReg1Type: " << srcReg1Type;
        errs() << "\t ptrSrcReg1: " << tmpPtrSrc1;
        errs() << "\t srcReg2: " << srcReg2;
        errs() << "\t srcReg2Type: " << srcReg2Type;
        errs() << "\t ptrSrcReg2: " << tmpPtrSrc2;
        errs() << "\n\n";
        }

        //Resetting almost all tmp variables to nullptr
        //destReg and operation are not reinitialized because they will be useful when a store operation in detected

        backUpSrcReg1Type = srcReg1Type;
        backUpSrcReg2Type = srcReg2Type;
        srcReg1 = nullptr;
        srcReg2 = nullptr;
        tmpPtrSrc1 = nullptr;
        tmpPtrSrc2 = nullptr;
        //tmpPtrDest = nullptr;
        isSrc1Array = false;
        isSrc2Array = false;
        srcReg1Type = InstructionTable::undefined;
        srcReg2Type = InstructionTable::undefined;


    } 
}

///It handles store instructions. It means that it updates lastUsedRegMap, detects if dest reg is an array and update infos related to it (tmpDestPtr)
void DependencyDetector::handleStoreInstruction(const Instruction &I){

    //Getting the store destination
    int * storeDest = (int*)(I.getOperand(1));

    //Getting the allocated register related to the store destination register
    int * allocatedReg = infoCollection.getAllocatedReg(storeDest);

    //Checking if the store destination is an array
    bool isAllocatedRegArray = infoCollection.isArray(allocatedReg);
    

    //If the allocated reg is an array
    if(isAllocatedRegArray){

        //storeDest is a pointer relative to the array in allocatedReg
        tmpPtrDest = storeDest;

        //The allocated reg relative to the dest reg is already contained in allocatedReg

    }else{
        //If the allocated reg is not an array, it refers to a SV

        //The allocated reg of dest reg is contained in storeDest (store destination corresponds to the allocated reg for a variable)
        allocatedReg = storeDest;

    }


    //destReg e operand(0) sono uguali
    if(debugMode){
        errs() << "srcReg1: " << srcReg1 << ", loadCount: " << loadCount << ", allocatedReg: " << allocatedReg << ", isAllocatedRegArray: " << isAllocatedRegArray <<"\n\n";
    }

    //Check if operation is not null
    if(operation != "null"){

        if(loadCount >= 2){
            //If two load operations have preceeded the store, an operation has been performed on the two operands loaded
            //Simple two operands operation like c = a + b

            //If more than 2 loads have been performed before the store, and the operation still is NOT a load
            //A multi-operand operation is being considered
            
            //Accumulation detection
            if(accumulationValidityCount >= 2 && tmpAccumulationOp == (int*)(I.getOperand(1))){

                IM.instructionMapIT = (IM.instructionMap).find(destReg);
                if(((IM.instructionMapIT)->second).isInLoopBody){

                    IM.addSpecToInstruction(destReg, "accumulation");

                    if(loadCount > 2){
                        IM.swapOperands(destReg);
                    }

                    if(isAllocatedRegArray){
                        IM.modifyGivenField(destReg, "destRegPtr", tmpPtrDest, InstructionTable::undefined);  
                        IM.modifyGivenField(destReg, "destRegType", nullptr, InstructionTable::array);  
                    }else{
                        IM.modifyGivenField(destReg, "destRegPtr", nullptr, InstructionTable::undefined);  
                        IM.modifyGivenField(destReg, "destRegType", nullptr, InstructionTable::singleVariable);  
                    }

                }       
      
            }else{

                if(isAllocatedRegArray){
                    IM.modifyGivenField(destReg, "destRegPtr", tmpPtrDest, InstructionTable::undefined);  
                    IM.modifyGivenField(destReg, "destRegType", nullptr, InstructionTable::array);  
                }else{
                    IM.modifyGivenField(destReg, "destRegPtr", nullptr, InstructionTable::undefined);  
                    IM.modifyGivenField(destReg, "destRegType", nullptr, InstructionTable::singleVariable);  
                }
                
            }
            
        }else if (loadCount == 1){

            if(operation == "load"){
                //If only one load has been performed before the store, and the operation variable still is a load,
                //an assignment like x[i] = a is being considered

                destReg = (int*)(I.getOperand(0));

                //If it is not an accumulation and dest reg is an array, its relative pointer must be inserted in IM
                if(isAllocatedRegArray){
                    IM.modifyGivenField(srcReg1, "destRegPtr", tmpPtrDest, InstructionTable::undefined);   
                    IM.modifyGivenField(srcReg1, "destRegType", nullptr, InstructionTable::array);     
                    IM.modifyGivenField(srcReg1, "srcReg2Type", nullptr, InstructionTable::array);    
                }else{
                    IM.modifyGivenField(srcReg1, "destRegPtr", nullptr, InstructionTable::undefined);  
                    IM.modifyGivenField(srcReg1, "destRegType", nullptr, InstructionTable::singleVariable); 
                    IM.modifyGivenField(srcReg1, "srcReg2Type", nullptr, InstructionTable::singleVariable);  
                }
                

                IM.modifyGivenField(srcReg1, "destReg", (int*)(I.getOperand(0)), InstructionTable::undefined);

                if(debugMode){
                    errs() << "Changed dest reg\n";
                }

            }else{
                //Operation involving a constant, such as c = a * 3
                
                //If it is not an accumulation and dest reg is an array, its relative pointer must be inserted in IM
                if(isAllocatedRegArray){
                    IM.modifyGivenField(destReg, "destRegPtr", tmpPtrDest, InstructionTable::undefined);   
                    IM.modifyGivenField(destReg, "destRegType", nullptr, InstructionTable::array);    
                }else{
                    IM.modifyGivenField(destReg, "destRegPtr", nullptr, InstructionTable::undefined);  
                    IM.modifyGivenField(destReg, "destRegType", nullptr, InstructionTable::singleVariable);  
                }

            }
            

        }else if (loadCount == 0){
            //TO BE HANDLED (constant)


        }

        //Check if store destination is already is lastUsedRegMap
        lastUsedRegMapIT = lastUsedRegMap.find(allocatedReg);

        //If it is not present, insert it with operand 0, which will become the last used reg associated to the allocatedReg
        if(lastUsedRegMapIT == lastUsedRegMap.end()){

            lastUsedRegMap.insert(std::pair<int*, int*>(allocatedReg, destReg));

        }else{

            //If it is present, insert the last used reg, that is operand 0
            lastUsedRegMapIT->second = destReg;

        }

        //Resetting tmp variables
        accumulationValidityCount = 0;
        loadCount = 0;
        srcReg1 = nullptr;
        srcReg2 = nullptr;
        destReg = nullptr;
        tmpPtrSrc1 = nullptr;
        tmpPtrSrc2 = nullptr;
        tmpPtrDest = nullptr;
        operation = "null";
        srcReg1Type = InstructionTable::undefined;
        srcReg2Type = InstructionTable::undefined;
        destRegType = InstructionTable::undefined;
        backUpSrcReg2Type = InstructionTable::undefined;
        backUpSrcReg1Type = InstructionTable::undefined;
        isSrc1Array = false;
        isSrc2Array = false;

    }


}

void DependencyDetector::detectDependencies(std::list<BasicBlock*> basicBlocksToSchedule){

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

    //operation string
    operation = "null";

    isSrc1Constant = false;
    isSrc2Constant = false;

    bool isNotNegated = true;

    errs() << "Begin scheduling instructions\n";

    //Cycling over all basic block that had been previouly recognized as useful for scheduling
    for(auto listBBIT = basicBlocksToSchedule.begin(); listBBIT != basicBlocksToSchedule.end(); ++listBBIT){

        //Getting current instruction
        for(Instruction &I : *(*listBBIT)){

            if(debugMode){
            errs() << "Identified instruction: " << I << "\n";
            }

            //Checking if its a valid instruction to schedule, i.e. load, store, binary operator
            if(infoCollection.isValidInst(I)){

                if(debugMode){
                errs() << "Instruction is valid!\n";
                }

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

                            
                            if(tmpLogicOperatorDestReg == (int*)(I.getOperand(0)) && CI->getSExtValue() == -1){
                                IM.changeOperationOfInstruction(destReg, changeLogic.getNegativeLogic(operation));
                                isNotNegated = false;
                            }
                        }

                        tmpLogicOperatorDestReg = (int*)(&I);

                    }else if(I.getOpcode() == Instruction::And){
                        operation = "and";

                        //Detection of NAND operation
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(I.getOperand(1))){
                            if(tmpLogicOperatorDestReg == (int*)(I.getOperand(0)) && CI->getSExtValue() == -1){
                                IM.changeOperationOfInstruction(destReg, changeLogic.getNegativeLogic(operation));
                                isNotNegated = false;
                            }
                        }

                        tmpLogicOperatorDestReg = (int*)(&I);
                        
                    }else if(I.getOpcode() == Instruction::Or){
                        operation = "or";

                        //Detection of NOR operation
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(I.getOperand(1))){
                            if(tmpLogicOperatorDestReg == (int*)(I.getOperand(0)) && CI->getSExtValue() == -1){
                                IM.changeOperationOfInstruction(destReg, changeLogic.getNegativeLogic(operation));
                                isNotNegated = false;
                            }
                        }

                        tmpLogicOperatorDestReg = (int*)(&I);
                        
                    }

                    if(isNotNegated){
                        //Managing the insertion of a new instruction that is not a load
                        handleInstructionInserting(I);
                    }else{
                        isNotNegated = true;
                    }                   

                }else if(dyn_cast<LoadInst>(&I)){

                    operation = "load";

                    //Initializing accumulation temp variable
                    tmpAccumulationOp = (int*)(I.getOperand(0));

                    //Increment the count relative to accumulation detection
                    accumulationValidityCount++;

                    //Getting the allocated register related to the loaded operand
                    int* allocatedReg = infoCollection.getAllocatedReg((int*)(I.getOperand(0)));


                    //int* opToBeLoaded = (infoCollection.isArray(allocatedReg)) ? allocatedReg : (int*)(I.getOperand(0));

                    InstructionTable::operandType oT = (infoCollection.isArray(allocatedReg)) ? InstructionTable::array : InstructionTable::singleVariable;

                    //Insert a new load instruction in instructionMap
                    IM.insertLoadInstruction(allocatedReg, oT);

                    if(loadCount % 2 == 0){

                        //If the allocated register refers to an array
                        if(infoCollection.isArray(allocatedReg)){
                            //Temp pointer is assigned the loaded operand, which is a pointer in case the allocated reg is an array
                            tmpPtrSrc1 = (int*)(I.getOperand(0));
                            isSrc1Array = true;
                        }

                        //Getting the real scr register
                        srcReg1 = getRealSrcReg(allocatedReg);

                    }else{

                        //If the allocated register refers to an array
                        if(infoCollection.isArray(allocatedReg)){
                            //Temp pointer is assigned the loaded operand, which is a pointer in case the allocated reg is an array
                            tmpPtrSrc2 = (int*)(I.getOperand(0));
                            isSrc2Array = true;
                        }
                        
                        //Getting the real scr register
                        srcReg2 = getRealSrcReg(allocatedReg);

                    }

                    //increment loadCount
                    loadCount++;

                }else if(dyn_cast<StoreInst>(&I)){
                    //The store operation is the one that concludes a load-op-store sequence,
                    //hence, info about lastUsedRegMap are updated here, also checking if the dedsstReg is an array,
                    //case in which info about its pointer must be inserted in IM

                    handleStoreInstruction(I);

                }
            }            
        }
    }

    if(debugMode){
    errs() << "\n\n\n\n\n\n\n\n";
    IM.printInstructionMap();
    errs() << "\n\n\n\n\n\n\n\n";
    }

    //reorder the internal instruction vector to IM, preparing for binding phase
    IM.reorderInstructionVector();

}


///Function useful to get the entire instructionMap built by the scheduling operation
InstructionTable DependencyDetector::getDependencyGraph(){
    return IM;
}