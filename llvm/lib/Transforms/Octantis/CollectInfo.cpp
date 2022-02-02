/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// Collection of information that will be exploited during the later phases of Octantis, such as scheduling and binding
//                      
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "CollectInfo.h"

//LLVM Include Files
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/User.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Bitstream/BitCodes.h" //Useful to identify the opcodes

#include "PointerInfoTable.h"
#include "LoopInfoTable.h"
#include "AccessPattern.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Default constructor
CollectInfo::CollectInfo()
{

}

///Function useful to parse alloca instructions
void CollectInfo::parseAllocaInstructions(BasicBlock* BB){
    errs() << "Begin parsing alloca Instructions\n";

    //Cycling over instructions in the BB
    for (Instruction &I : *BB) {

        //Checking if the instruction in an alloca
        if(AllocaInst* a = dyn_cast<AllocaInst>(&I)){

            errs() << "Identified alloca instruction " << (int*)(&I) << "\n";

            //variables for number of rows and columns of an eventual array
            int rows, cols;  

            //getting the allocated type       
            Type *ty=(a->getAllocatedType());

            //Checking if the allocated type is an ArrayType
            if(ty->isArrayTy()){

                //cast the allocated type to ArrayType
                ArrayType *r = cast<ArrayType> (ty);

                //saving the number of elements of array
                cols = r->getNumElements();

                Type *ty1 = r->getElementType();
                if(ty1->isArrayTy()){

                    //get the internal array in case of 2-dimensional arrays
                    ArrayType *c = cast<ArrayType> (r->getElementType()); 

                    //saving the retrieved value
                    rows = c->getNumElements(); 

                }else{

                    rows = 1;

                } 

                //insert array information inside arraysInfoMap
                std::list<int> sizeList;
                sizeList.push_back(rows);
                sizeList.push_back(cols);
                arraysInfoMap.insert(std::pair<int*, std::list<int>> ((int*)&I, sizeList));
                errs() << "\tArray allocation identified with " << rows << " rows and " << cols << " columns\n";

            }

            //Insert an element in the aliasInfoMap, whose key field is the allocated register
            //The value field will be filled with all its aliases
            std::list<int*> tmpList;
            aliasInfoMap.insert(std::pair<int*, std::list<int*>>((int*)(&I), tmpList));

        }
    }
}

///Function useful to parse loop preheader in order to detect iterator initialization
void CollectInfo::parseLoopPreheader(BasicBlock* &BB){

    errs() << "Begin parsing Loop Preheader in search of initializations\n";

    //Cycling over BB instructions
    for (Instruction &I : *BB) {

        //Check if instruction is a store
        if(dyn_cast<StoreInst>(&I)){

            //Check if the operand to store is a constant
            if(ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(0))){

                //If it is a constant, then it MAY represent the initial value of an iterator
                //errs() << "\tIdentified initialization with store instruction,\n\t inserting in initValuesMap " << I.getOperand(1) << " with " << CI->getSExtValue() << "\n";
                initValuesMap.insert(std::pair<int *, int>((int*)(I.getOperand(1)),CI->getSExtValue()));

            }else{

                //If the operand to store is not a constant, a check is made in order to know if the initial value of the loop iterator
                //depends on another iterator already identified
                for(iteratorsAliasMapIT = iteratorsAliasMap.begin(); iteratorsAliasMapIT != iteratorsAliasMap.end(); ++iteratorsAliasMapIT){

                    if(iteratorsAliasMapIT->second == (int*)(I.getOperand(0))){
                        tmpVariableInitIterator = (int*)(I.getOperand(0));
                    }

                }

            }

        //Check if instruction is a store
        }else if(dyn_cast<LoadInst>(&I)){
            
            //Checking if the operand to be loaded is the allocated register of an iterator
            iteratorsAliasMapIT = iteratorsAliasMap.find((int*)I.getOperand(0));

            if(iteratorsAliasMapIT != iteratorsAliasMap.end()){
                //If yes, the load destination is inserted as the operand to be loaded aliass
                iteratorsAliasMapIT->second = (int*)(&I);
            }
        }

    }
}

///Function devoted to the collection of information regarding loops
void CollectInfo::collectLoopInfo(LoopInfo &LI){

    //Getting the loops
    SmallVector<Loop *, 4> lip = LI.getLoopsInPreorder();

    std::vector<Loop* > Loops;

    BasicBlock * loopHeader;
    BasicBlock * loopLatch;
    BasicBlock * loopPreheader;

    int* iteratorOp;
    int depth;
    Loop* parent;
    int* parentIt;

    //Iterating over loops in function
    for(Loop** lipIT = lip.begin(); lipIT != lip.end(); ++lipIT){

        //Retrieving loop preheader, loop header and loop latch of the current loop
        loopPreheader = (*lipIT)->getLoopPreheader();
        loopHeader = (*lipIT)->getHeader();
        loopLatch = (*lipIT)->getLoopLatch();

        //Push them back in their respective lists
        loopPreheaderList.push_back((int*)(loopPreheader));
        loopHeaderList.push_back((int*)(loopHeader));
        loopLatchList.push_back((int*)(loopLatch));

        errs() << "Found loop " << (int*)(*lipIT) << " with Preheader: " << loopPreheader << 
                " with Header: " << loopHeader << " and Latch: " << loopLatch << "\n";

        //Parse loop preheader
        parseLoopPreheader(loopPreheader);

        //Parsing both loop header and loop latch, returning the iterator of the current loop
        iteratorOp = parseLoopHeaderAndLatch(loopHeader, loopLatch);

        //Inserting the pair <current loop, its iterator> inside loopIteratorMap
        loopIteratorMap.insert(std::pair<Loop*, int*> (*lipIT, iteratorOp));

        //Insert a new loop in LIT
        LIT.insertNewLoopIt(iteratorOp);

        //Retrieving the depth of the current loop
        depth = (*lipIT)->getLoopDepth();

        //Initialize parent to current loop
        parent = (*lipIT);

        //Knowing the depth, retrieving all the loop parents in reverse order and push them into the proper structure inside LIT
        for(int i = 1; i < depth; ++i){

            //Getting the parent
            parent = parent->getParentLoop();

            //Retrieve the parent loop iterator
            parentIt = (loopIteratorMap.find(parent))->second;

            //Push it in the LIT data structure
            LIT.pushNestedLoop(iteratorOp, parentIt);

        }

    }
}

///Function to parse Loops information
int* CollectInfo::parseLoopHeaderAndLatch(BasicBlock* &LH, BasicBlock* &LL){

    errs() << "Begin parsing Loop Header info\n";

    LoopInfoTable::loopInfoStruct loopInfo;
    int* it;
    loopInfo.loopHeader = (int*) LH;
    loopInfo.loopLatch = (int*) LL;

    //Parsing the internal instructions
    for (Instruction &I : *LH) {

        Instr i = identifyInstr(I);

        errs() << "\tFetched (inside Loop Header): " << I << "; recognized: " << i << "\n";
        switch(i)
        {
            case load:
            {
                //Find the initial value of the operand to be loaded
                initValuesMapIT = initValuesMap.find((int*)I.getOperand(0));

                //errs() << "\tFinding in initValuesMap " << I.getOperand(0) << "\n";
                if (initValuesMapIT != initValuesMap.end()){

                    //If found, the operand is looked for into the iteratorsAliasMap
                    iteratorsAliasMapIT = iteratorsAliasMap.find((int*)I.getOperand(0));

                    if(iteratorsAliasMapIT == iteratorsAliasMap.end()){

                        //If it is not present, add it to iteratorAliasMap, removing it from aliasInfoMap
                        //errs() << "inserting in iteratorsAliasMap " << initValuesMapIT->first << "\n";
                        iteratorsAliasMap.insert(std::pair<int*, int*> (initValuesMapIT->first, (int*)(&I)));
                        aliasInfoMapIT = aliasInfoMap.find((int*)(I.getOperand(0)));
                        aliasInfoMap.erase(aliasInfoMapIT);

                        //Setting the iterator initial value because now it is sure that the variable initialized was an iterator
                        loopInfo.iteratorInitValue = initValuesMapIT->second;
                        loopInfo.isInitValueConstant = true;

                        //iterator of the current loop header to be inserted as first field of loopInfoMap
                        it = initValuesMapIT->first;

                    }else{

                        //If it is already present, modify the alias value with the the new one, the load destination
                        iteratorsAliasMapIT->second = (int*)(&I);
                        
                    }

                }else{

                    //If it is not present in initValuesMap, the same checks are performed but the initial value is
                    //not constant, it depends on another iterator

                    iteratorsAliasMapIT = iteratorsAliasMap.find((int*)I.getOperand(0));
                    if(iteratorsAliasMapIT == iteratorsAliasMap.end()){

                        //Add iterator to iteratorAliasMap, removing it from aliasInfoMap
                        //errs() << "inserting in iteratorsAliasMap " << initValuesMapIT->first << "\n";
                        iteratorsAliasMap.insert(std::pair<int*, int*> (initValuesMapIT->first, (int*)(&I)));
                        aliasInfoMapIT = aliasInfoMap.find((int*)(I.getOperand(0)));
                        aliasInfoMap.erase(aliasInfoMapIT);

                        //Setting the iterator initial value because now it is sure that the variable initialized was an iterator
                        loopInfo.variableInitValue = tmpVariableInitIterator;
                        loopInfo.isInitValueConstant = false;

                        //iterator of the current loop header to be inserted as first field of loopInfoMap
                        it = initValuesMapIT->first;

                    }else{
                        
                        //If it is already present, modify the alias value with the the new one, the load destination
                        iteratorsAliasMapIT->second = (int*)(&I);
                        
                    }

                }

            }
                break;

            case icmp:
            {
                //Condition through which determine the iterator final value
                Value *iter = I.getOperand(1);

                //Check if the final value is constant
                if(ConstantInt* CI = dyn_cast<ConstantInt>(iter)){
                    
                    //If the operand is a constant, the final iterator value must be assigned
                    //The predicate (slt, sgt, sle, sge) must be taken into account in order to properly assign the number of iterations
                    ICmpInst *cmpInstr = cast<ICmpInst>(&I);
                    StringRef icmpPredicate = cmpInstr->getPredicateName(cmpInstr->getPredicate());

                    loopInfo.iteratorFinalValue = CI->getSExtValue();

                    //Calculating the loop iterations
                    if(icmpPredicate == "sge" || icmpPredicate == "sle"){
                        loopInfo.iterations = loopInfo.iteratorFinalValue - loopInfo.iteratorInitValue + 1;
                    }else if(icmpPredicate == "sgt" || icmpPredicate == "slt"){
                        loopInfo.iterations = loopInfo.iteratorFinalValue - loopInfo.iteratorInitValue;
                    }

                    loopInfo.isFinalValueConstant = true;

                    //errs() << "\tThe loop iterator final value is " << loopInfo.iteratorFinalValue << "\n";

                }else{
                    //If it is not constant, the final value is look for into the iteratorsAliasMap in order to check
                    //if it is a iterator

                    for(iteratorsAliasMapIT = iteratorsAliasMap.begin(); iteratorsAliasMapIT != iteratorsAliasMap.end(); ++iteratorsAliasMapIT){

                        if(iteratorsAliasMapIT->second == (int*) iter){

                            loopInfo.isFinalValueConstant = false;
                            loopInfo.variableFinalValue = (int*)(iter);

                        }
                    }
                }                

                //errs() << "The iterations of the loop statement are: " << loopInfo.iterations << "\n";
            }
                break;

            case branch:
            {
                //Identification of the bodies BB: one is given in the branch condition, the other part of the loop body is 
                //given by the BB before the loop patch

                int* LB1 = (int *)(I.getOperand(2));
                int* LB2 = (int *)(LL->getSinglePredecessor());

                (loopInfo.loopBody).push_back(LB1);
                (loopInfo.loopBody).push_back(LB2);

                loopBodyList.push_back(LB1);
                loopBodyList.push_back(LB2);

                //errs() << "\tLoop bodies of the loop are: " << LB1 << " and " << LB2 << "\n";

            }
                break;

            default:
            {
                llvm_unreachable("Collect Info error: instruction inside Loop Header not identified\n");
            }

        }

    }

    
    errs() << "Starting to parse Loop Latch Info...\n";

    // Parsing the internal instructions
    for (Instruction &I : *LL) {

        Instr i=identifyInstr(I);
        errs()<<"Fetched (inside Loop Latch): " << I << "; recognized: " << i << "\n";
        switch(i)
        {
            case binary:
            {

                //ASSUMPTION: the increment is constant!!!

                //If an addition is present, in order to know the itearator increment
                //Check if also Sub is useful to be identified!
                if(I.getOpcode() == Instruction::Add){
                    //Increment constant extraction
                    Value *iter = I.getOperand(1);
                    ConstantInt* CI = dyn_cast<ConstantInt>(iter);                

                    loopInfo.iteratorIncrement = CI->getSExtValue();

                    //errs() << "\tThe iterator increment is: " << CI->getSExtValue() << "\n";
                }         
            }
                break;

            default:
            {
                break;
            }

        }
    }
    
    
    //errs() << "Inserting new loop and its iterator in loopInfoMap: iterator " << it << "\n";

    //Insert the retrieved info inside LoopInfoTable
    LIT.insertLoopInfo(it, loopInfo);

    //Resetting loopInfo
    LoopInfoTable::loopInfoStruct emptyLoopInfo;
    loopInfo = emptyLoopInfo;

    return it;
    
}

///Function to parse useful information inside a loop's body
void CollectInfo::parseLoopBody(BasicBlock &BB){

    errs() << "Begin parsing Loop Body info\n";

    // Parsing the internal instructions
    for (Instruction &I : BB) {

        Instr i=identifyInstr(I);
        errs()<<"Fetched (inside Loop Body): " << I << "; recognized: " << i << "\n";
        switch(i)
        {
            case store:
            {
                //Extracting first operand of store instruction and checking whether it is a constant value
                Value *value = I.getOperand(0);

                if(ConstantInt* CI = dyn_cast<ConstantInt>(value)){
                    //If it is a constant, insert it in the relative map
                    initValuesMap.insert(std::pair<int *, int>((int*)(I.getOperand(1)),CI->getSExtValue()));
                }

            }
                break;

            case load:
            {

                //Check if the loaded register refers to an iterator
                iteratorsAliasMapIT = iteratorsAliasMap.find((int*)I.getOperand(0));

                if (iteratorsAliasMapIT != iteratorsAliasMap.end()){
                    //substituting old alias with new one
                    //errs() << "adding iterator " << (int*)(&I) << " as alias of " << iteratorsAliasMapIT->first << "\n";
                    iteratorsAliasMapIT->second = (int*)(&I);

                }else{
                    //Update aliasInfoMap with register aliases
                    aliasInfoMapIT = aliasInfoMap.find((int*)(I.getOperand(0)));
                    
                    if(aliasInfoMapIT != aliasInfoMap.end()){
                        //Alias found and to be added to aliasInfoMap
                        (aliasInfoMapIT->second).push_back((int*)(&I));
                    }
                }

            }
                break;

            case binary:
            {
                //If an addition is present, it may be between 2 iterators
                //Check if also Sub is useful to be identified!
                if(I.getOpcode() == Instruction::Add || I.getOpcode() == Instruction::Sub){
                    Value* op1 = I.getOperand(0);
                    Value* op2 = I.getOperand(1);

                    bool firstOpFound = false;
                    bool secondOpFound = false;

                    int* firstOpIt;
                    int* secondOpIt;

                    //Checking whether the operands are related to iterators
                    for(iteratorsAliasMapIT = iteratorsAliasMap.begin(); iteratorsAliasMapIT != iteratorsAliasMap.end(); ++iteratorsAliasMapIT){
                        if(iteratorsAliasMapIT->second == (int*) op1){

                            //The first operand is an iterator
                            firstOpFound = true;
                            //Getting its allocatedReg
                            firstOpIt = iteratorsAliasMapIT->first;

                        }else if(iteratorsAliasMapIT->second ==  (int*) op2){

                            //The second operand is an iterator
                            secondOpFound = true;
                            //Getting its allocatedReg
                            secondOpIt = iteratorsAliasMapIT->first;

                        }
                    }

                    //If an addition operation between 2 iterators is identified
                    if(firstOpFound && secondOpFound){
                        
                        std::list<int*> itList;
                        itList.push_back(firstOpIt);
                        itList.push_back(secondOpIt);

                        PointerInfoTable::indexInfoStruct idxInfo;
                        idxInfo.constant = 0;
                        idxInfo.iterators = itList;
                        idxInfo.operation = I.getOpcode();

                        combinedIteratorsMap.insert(std::pair<int*, PointerInfoTable::indexInfoStruct> ((int*)(&I), idxInfo));


                    }else if(firstOpFound && !secondOpFound){

                        //If the first operand is an iterator and the second operand is a constant
                        if(ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(1))){
                            std::list<int*> itList;
                            itList.push_back(firstOpIt);

                            PointerInfoTable::indexInfoStruct idxInfo;
                            idxInfo.constant = (I.getOpcode() == Instruction::Add) ? CI->getSExtValue() : -CI->getSExtValue();
                            idxInfo.iterators = itList;
                            idxInfo.operation = I.getOpcode();

                            combinedIteratorsMap.insert(std::pair<int*, PointerInfoTable::indexInfoStruct> ((int*)(&I), idxInfo));

                        }

                    }
                }

            }
                break;

            case ptr:
            {

                GetElementPtrInst *GEP = cast<GetElementPtrInst>(&I);

                //Finding the array pointer inside arraysInfoMap
                arraysInfoMapIT = arraysInfoMap.find((int*)(GEP->getPointerOperand()));

                if(arraysInfoMapIT != arraysInfoMap.end()){
                    //The pointer refers to a previously detected array
                    //errs() << "Array found!\n";
                    //Check if the index is a combination of multiple loop iterators
                    combinedIteratorsMapIT = combinedIteratorsMap.find((int*)(GEP->getOperand(2)));

                    if(combinedIteratorsMapIT != combinedIteratorsMap.end()){
                        //errs() << "\t\tCombined iterator found\n";
                        //Index is a combination of iterators

                        //If the information about the size is empty, it means that the second index will be considered
                        if((arraysInfoMapIT->second).empty()){
                            //second index
                            //errs() << "Considering second idx\n";
                            tmpPointerStruct.secondIdxInfo = combinedIteratorsMapIT->second;
                            ptrValid = true;
                        }else{
                            //first index
                            tmpPointerStruct.firstIdxInfo = combinedIteratorsMapIT->second;

                        }
                    }else if (!(dyn_cast<ConstantInt>(GEP->getOperand(2)))){
                        //index is not a combination of iterators, not a constant, check if it is made up of a single iterator
                        for(iteratorsAliasMapIT = iteratorsAliasMap.begin(); iteratorsAliasMapIT != iteratorsAliasMap.end(); ++iteratorsAliasMapIT){
                            if(iteratorsAliasMapIT->second == (int*)(GEP->getOperand(2))){
                                //If the information about the size is empty, it means that the second index will be considered
                                if((arraysInfoMapIT->second).empty()){
                                    //second index
                                    PointerInfoTable::indexInfoStruct idxInfo;
                                    std::list<int*> itList;
                                    std::map<int*, int> itMap;

                                    itList.push_back(iteratorsAliasMapIT->first);
                                    itMap.insert(std::pair<int*, int> (iteratorsAliasMapIT->first, 0));

                                    idxInfo.constant = 0;
                                    idxInfo.operation = 0;
                                    idxInfo.iterators = itList;
                                    tmpPointerStruct.secondIdxInfo = idxInfo;

                                    ptrValid = true;
                                }else{
                                    //first index
                                    PointerInfoTable::indexInfoStruct idxInfo;
                                    std::list<int*> itList;
                                    std::map<int*, int> itMap;

                                    itList.push_back(iteratorsAliasMapIT->first);
                                    itMap.insert(std::pair<int*, int> (iteratorsAliasMapIT->first, 0));

                                    idxInfo.constant = 0;
                                    idxInfo.operation = 0;
                                    idxInfo.iterators = itList;
                                    tmpPointerStruct.firstIdxInfo = idxInfo;

                                }
                            }
                        }
                    }else if(ConstantInt* CI = dyn_cast<ConstantInt>(GEP->getOperand(2))){
                        //If the information about the size is empty, it means that the second index will be considered
                        if((arraysInfoMapIT->second).empty()){
                            //second index
                            PointerInfoTable::indexInfoStruct idxInfo;
                            std::list<int*> itList;

                            idxInfo.constant = CI->getSExtValue();
                            idxInfo.operation = 0;
                            idxInfo.iterators = itList;
                            tmpPointerStruct.secondIdxInfo = idxInfo;

                            ptrValid = true;
                        }else{
                            //first index
                            PointerInfoTable::indexInfoStruct idxInfo;
                            std::list<int*> itList;

                            idxInfo.constant = CI->getSExtValue();
                            idxInfo.operation = 0;
                            idxInfo.iterators = itList;
                            tmpPointerStruct.firstIdxInfo = idxInfo;

                        }
                    }

                    //If the array is 1-dim array, all infos are already available
                    if((arraysInfoMapIT->second).front() == 1){
                        PointerInfoTable::indexInfoStruct emptyIdxInfo;
                        tmpPointerStruct.secondIdxInfo = emptyIdxInfo;

                        //errs() << "Inserting " << (int*)(&I) << " in PIT with first index " << ((tmpPointerStruct.firstIdxInfo).iterators).front() << "\n";
                        PIT.insertPointerInfo((int*)(&I), tmpPointerStruct);

                        //update aliasInfoMap
                        aliasInfoMapIT = aliasInfoMap.find((int*)(GEP->getPointerOperand()));
                        if(aliasInfoMapIT != aliasInfoMap.end()){
                            (aliasInfoMapIT->second).push_back((int*)(&I));
                        }
                        
                        setPointerInfo((int*)(&I), &I);

                    }else{
                        if(ptrValid){

                            //errs() << "update" << getAllocatedReg((int*)(GEP->getPointerOperand())) << " GEP " << (int*)(GEP->getPointerOperand()) << "\n";

                            ptrValid = false;
                            PIT.insertPointerInfo((int*)(&I), tmpPointerStruct);

                            //update aliasInfoMap
                            aliasInfoMapIT = aliasInfoMap.find(getAllocatedReg((int*)(GEP->getPointerOperand())));
                            if(aliasInfoMapIT != aliasInfoMap.end()){
                                (aliasInfoMapIT->second).push_back((int*)(&I));
                            }

                            setPointerInfo((int*)(&I), &I);

                            //Erasing previous pointer which is useless
                            /*for(auto listIT = (aliasInfoMapIT->second).begin(); listIT != (aliasInfoMapIT->second).end(); ++listIT){
                                if(*listIT == (int*)(GEP->getPointerOperand())){
                                    (aliasInfoMapIT->second).erase(listIT);
                                }
                            }*/

                        }else{
                            std::list<int> emptyList;
                            arraysInfoMap.insert(std::pair<int*, std::list<int>> ((int*)(&I), emptyList));

                            //update aliasInfoMap
                            aliasInfoMapIT = aliasInfoMap.find((int*)(GEP->getPointerOperand()));
                            if(aliasInfoMapIT != aliasInfoMap.end()){
                                //errs() << "Inserting in aliasInfo map " << aliasInfoMapIT->first << " " << (int*)(&I);
                                (aliasInfoMapIT->second).push_back((int*)(&I));
                            }
                        }
                    }

                }

            }
                break;

            case sext:
            {
                //Check if the operand of the sext is a combination of iterators/iterator+constant
                combinedIteratorsMapIT = combinedIteratorsMap.find((int*)(I.getOperand(0)));

                if(combinedIteratorsMapIT != combinedIteratorsMap.end()){
                    //Save the found element in a tmp variable
                    std::pair<int*, PointerInfoTable::indexInfoStruct> tmpEl = *combinedIteratorsMapIT;

                    tmpEl.first = (int*)(&I);
                    
                    combinedIterators.push_back(combinedIteratorsMapIT->first);

                    //Replace the old element with the new having the updated operand
                    combinedIteratorsMap.erase(combinedIteratorsMapIT);
                    combinedIteratorsMap.insert(tmpEl);

                }else{
                    for(iteratorsAliasMapIT = iteratorsAliasMap.begin(); iteratorsAliasMapIT != iteratorsAliasMap.end(); ++iteratorsAliasMapIT){
                        if(iteratorsAliasMapIT->second == ((int*)(I.getOperand(0)))){

                            //substituting old alias with new one

                            //errs() << "adding iterator " << (int*)(&I) << " as alias of " << iteratorsAliasMapIT->first << "\n";
                            iteratorsAliasMapIT->second = (int*)(&I);
                        }
                    }
                }

            }
                break;

            default:
            {
                break;
            }
        }
    }
}

///Function that parse, collecting informations, the given Function in order to pass only the essential info to the scheduler
void CollectInfo::parseFunction(Function &F, LoopInfo &LI){

    bool isLoopHeader = false;
    bool isLoopLatch = false;
    bool isLoopBody = false;
    bool isFirstBB = true;

    for (BasicBlock &BB : F) {
        errs() << "\t\tBasic Block: " << (int*)&BB << " successor " <<(int *)(BB.getSingleSuccessor()) << " size " << BB.size() << "\n";      

        if(isFirstBB){
            isFirstBB = false;
            parseAllocaInstructions(&BB);
            validBBs.push_back(&BB);
            collectLoopInfo(LI);
        }else{
            for(auto listIT = loopHeaderList.begin(); listIT != loopHeaderList.end(); ++listIT){
                if(*listIT == (int*)(&BB)){
                    //current BB is a loop header
                    isLoopHeader = true;
                }
            }

            for(auto listIT = loopLatchList.begin(); listIT != loopLatchList.end(); ++listIT){
                if(*listIT == (int*)(&BB)){
                    //current BB is a loop latch
                    isLoopLatch = true;
                }
            }

            for(auto listIT = loopBodyList.begin(); listIT != loopBodyList.end(); ++listIT){
                if(*listIT == (int*)(&BB)){
                    //current BB is a loop body
                    isLoopBody = true;
                }
            }

            if(isLoopBody){
                //Parse loop body and put it in the list of valid BBs
                parseLoopBody(BB);
                validBBs.push_back(&BB);
            }else if(!isLoopHeader && !isLoopLatch){
                //If the BB is neither a loop header or a loop latch (and it is not a loop body),
                //it means it refers to a valid block (for scheduling) outside of a loopù
                parseLoopBody(BB);
                validBBs.push_back(&BB);
            }

            isLoopHeader = false;
            isLoopLatch = false;
            isLoopBody = false;
        }
        
    }

    errs() << "valid BBs are:\n";
    for(auto it = validBBs.begin(); it != validBBs.end(); ++it){
        errs() << "\t\tBasic Block: " << (int*)(*it) << " with size: " << (*it)->size() << "\n";
    }
}

///Function useful to know if the valid BB instruction must be considered for scheduling
bool CollectInfo::isValidInst(Instruction &I){
    bool changed = false;

    if(StoreInst *SI = dyn_cast<StoreInst>(&I)){
        //If the destination of the store instruction is an iterator-related register, it must be erased
        if(iteratorsAliasMap.find((int*)(SI->getOperand(1))) != iteratorsAliasMap.end()){
            changed = true;
        }
    }else if(LoadInst *LI = dyn_cast<LoadInst>(&I)){
        //If the loaded register is an iterator, the instruction must be removed
        if(iteratorsAliasMap.find((int*)(LI->getOperand(0))) != iteratorsAliasMap.end()){
            changed = true;
        }
    }else if(dyn_cast<SExtInst>(&I)){
        changed = true;
    }else if(dyn_cast<GetElementPtrInst>(&I)){
        changed = true;
    }else if(BinaryOperator *BO = dyn_cast<BinaryOperator> (&I)){
        if(BO->getOpcode() == Instruction::Add){
            //If the destination register is a combination of iterators, the instruction must be removed
            for(auto listIT = combinedIterators.begin(); listIT != combinedIterators.end(); ++listIT){
                if(*listIT == (int*)(&I)){
                    changed = true;
                }
            }
        }else if(BO->getOpcode() == Instruction::Sub){
            //If the destination register is a combination of iterators, the instruction must be removed
            for(auto listIT = combinedIterators.begin(); listIT != combinedIterators.end(); ++listIT){
                if(*listIT == (int*)(&I)){
                    changed = true;
                }
            }
        }
    }else if(dyn_cast<BranchInst>(&I)){
        changed = true;
    }

    return !(changed);
}

///Function useful to retrieved the allocated register related to the input alias operand
int * CollectInfo::getAllocatedReg(int * op){
    for(aliasInfoMapIT = aliasInfoMap.begin(); aliasInfoMapIT != aliasInfoMap.end(); ++aliasInfoMapIT){
        for(auto listIT = (aliasInfoMapIT->second).begin(); listIT != (aliasInfoMapIT->second).end(); ++listIT){
            if(*listIT == op){
                //Retrieve allocated register
                return aliasInfoMapIT->first;
            }
        }
    } 

    return nullptr;
}

///Function useful to get the size of the array/matrix given in input the allocated reg
int CollectInfo::getArraySize(int *reg){
    arraysInfoMapIT = arraysInfoMap.find(reg);
    if(arraysInfoMapIT == arraysInfoMap.end()){
        return 1;
    }else{
        return ((arraysInfoMapIT->second).front())*((arraysInfoMapIT->second).back());
    }
}

///Function useful to get the number of columns of the array
int CollectInfo::getArrayCols(int *reg){
    arraysInfoMapIT = arraysInfoMap.find(reg);
    if(arraysInfoMapIT == arraysInfoMap.end()){
        return 0;
    }else{
        return (arraysInfoMapIT->second).back();
    }
}

///Function useful to get the number of rows of the array
int CollectInfo::getArrayRows(int *reg){
    arraysInfoMapIT = arraysInfoMap.find(reg);
    if(arraysInfoMapIT == arraysInfoMap.end()){
        return 0;
    }else{
        return (arraysInfoMapIT->second).front();
    }
}

///Function useful to know if the current BB is a loop's body
bool CollectInfo::isLoopBody(const BasicBlock* BB){

    for(auto listIT = loopBodyList.begin(); listIT != loopBodyList.end(); ++listIT){
        if(*listIT == (int*)BB){
            //current BB is a loop body
            return true;
        }
    }

    return false;

}

///Function useful to check the operand refers to an array
bool CollectInfo::isArray(int* operand){
    if(arraysInfoMap.find(operand) != arraysInfoMap.end()){
        return true;
    }

    return false;
}

///Function useful to set useful parameters for the binding phase concerning arrays and their indexes
void CollectInfo::setPointerInfo(int* pointer, Instruction* I){

    errs() << "Starting setPointerInfo\n";

    PointerInfoTable::pointerInfoStruct pointerInfo;
    //Retrieving the info about the array (pointer)
    pointerInfo = PIT.getPointerInfo(pointer);

    PointerInfoTable::indexInfoStruct firstIdxInfo = pointerInfo.firstIdxInfo;
    PointerInfoTable::indexInfoStruct secondIdxInfo = pointerInfo.secondIdxInfo;

    //Getting the loop body basic block
    BasicBlock* instrLoopBody = I->getParent();

    //errs() << "\tLoop Body " << instrLoopBody << "\n";
    //Getting the iterator associated to the loop whose loop body is instrLoopBody
    int* loopIteratorOp = LIT.getLoopIteratorFromLoopBody((int*)(instrLoopBody));

    pointerInfo.loopIterator = loopIteratorOp;

    //Getting the nested structure of loops in which this loop is present
    std::pair<int*, std::list<int*>> nestedLoops = LIT.getNestedLoops(loopIteratorOp); 

    //List of loops iterator of loops containing the loopIteratorOp loop
    std::list<int*> higherLevelLoops = nestedLoops.second;

    //Iteration domain
    std::list<int*> iterationVector = LIT.getIterationVector(loopIteratorOp);

    //errs() << "\tLoop iterator " << loopIteratorOp << " higher level one " << higherLevelLoops.front() << "\n";

    //Finding the allocated reg relative to the current array pointer
    //and then search in arraysInfoMap to retrieve rows and columns of the entire array
    int* allocatedArrayReg = getAllocatedReg(pointer);

    //errs() << "\tAllocated reg " << allocatedArrayReg << "\n";
    std::list<int> arraySizes = (arraysInfoMap.find(allocatedArrayReg))->second;

    //errs() << "\tAllocated reg size rows " << arraySizes.front() << " size cols " << arraySizes.back() << "\n";
    int arrayRows = arraySizes.front();
    //int arrayCols = arraySizes.back();

    //errs() << "\tfirst index it: " << (firstIdxInfo.iterators).front() << " " << (firstIdxInfo.iterators).size() << "\n";
    //errs() << "\tsecond index it: " << (secondIdxInfo.iterators).front() <<  " " << (secondIdxInfo.iterators).size() << "\n";
    

    std::list<PointerInfoTable::indexInfoStruct> indexInfoList;
    if(arrayRows > 1){
        indexInfoList.push_back(pointerInfo.firstIdxInfo);
        indexInfoList.push_back(pointerInfo.secondIdxInfo);
    }else{
        indexInfoList.push_back(pointerInfo.firstIdxInfo);
    }
    


    if(arrayRows > 1){
        //arrayOp refers to a matrix

        if((firstIdxInfo.iterators).size() == 1 && (secondIdxInfo.iterators).size() == 1){
            //The two array indexes are composed by only one iterator

            //Retrieving the iterators of the first and the second index
            int* firstIdxIt = (firstIdxInfo.iterators).front();
            int* secondIdxIt = (secondIdxInfo.iterators).front();

            //Retrieving loop infos of each iterator
            LoopInfoTable::loopInfoStruct LISIt1 = LIT.getLoopInfo(firstIdxIt);
            LoopInfoTable::loopInfoStruct LISIt2 = LIT.getLoopInfo(secondIdxIt);

            if(LISIt1.isFinalValueConstant && LISIt1.isInitValueConstant && LISIt2.isFinalValueConstant && LISIt2.isInitValueConstant){
                //The two iterators bounds (init value and final value) are integer numbers

                if(firstIdxIt == loopIteratorOp && std::find(higherLevelLoops.begin(), higherLevelLoops.end(), secondIdxIt) != higherLevelLoops.end()){

                    /*
                        for(secondIdx){
                            for(firstIdx){
                                array[firstIdx][secondIdx]
                            }
                        }
                    */

                    pointerInfo.colFirst = true;
                    pointerInfo.rowFirst = false;
                    pointerInfo.setCols =  LISIt2.iterations;
                    pointerInfo.setRows = LISIt1.iterations;
                    pointerInfo.numberOfSets = 1;
                }else if(secondIdxIt == loopIteratorOp && std::find(higherLevelLoops.begin(), higherLevelLoops.end(), firstIdxIt) != higherLevelLoops.end()){
                    /*
                        for(firstIdx){
                            for(secondIdx){
                                array[firstIdx][secondIdx]
                            }
                        }
                    */

                    pointerInfo.colFirst = false;
                    pointerInfo.rowFirst = true;
                    pointerInfo.setCols = LISIt2.iterations;
                    pointerInfo.setRows = LISIt1.iterations;
                    pointerInfo.numberOfSets = 1;

                }
            }else{
                
                auto higherLevelLoopsIT = higherLevelLoops.begin();
                std::advance(higherLevelLoopsIT, 1);
                LoopInfoTable::loopInfoStruct LISIt3 = LIT.getLoopInfo(*higherLevelLoopsIT);
                LoopInfoTable::loopInfoStruct LISIt4 = LIT.getLoopInfo(higherLevelLoops.back());


                if(firstIdxIt == loopIteratorOp && secondIdxIt == higherLevelLoops.front()){

                    /*
                        for(secondIdx){
                            for(firstIdx){
                                array[firstIdx][secondIdx]
                            }
                        }
                    */

                    pointerInfo.colFirst = true;
                    pointerInfo.rowFirst = false;
                    pointerInfo.setCols =  -1;
                    pointerInfo.setRows = -1;
                    pointerInfo.numberOfSets = LISIt3.iterations * LISIt4.iterations;
                }else if(secondIdxIt == loopIteratorOp && firstIdxIt == higherLevelLoops.front()){
                    /*
                        for(firstIdx){
                            for(secondIdx){
                                array[firstIdx][secondIdx]
                            }
                        }
                    */

                    pointerInfo.colFirst = false;
                    pointerInfo.rowFirst = true;
                    pointerInfo.setCols =  -1;
                    pointerInfo.setRows = -1;
                    pointerInfo.numberOfSets = LISIt3.iterations * LISIt4.iterations;

                }
            }

        }else if((firstIdxInfo.iterators).size() == 2 && (secondIdxInfo.iterators).size() == 2){
            //The two array indexes are composed by two iterators each

            //Retrieving the iterators of the first and the second index
            int* it1  = (firstIdxInfo.iterators).front(); //first iterator of the first index
            int* it2  = (firstIdxInfo.iterators).back(); //second iterator of the first index
            int* it3 = (secondIdxInfo.iterators).front(); //first iterator of the second index
            int* it4 = (secondIdxInfo.iterators).back(); //second iterator of the second index

            //If both operations between iterators of index are additions, i.e. [i + k][j + l]
            if(firstIdxInfo.operation == Instruction::Add && secondIdxInfo.operation == Instruction::Add){
                
                //Retrieving the depth of the loop associated to each iterator (Outermost loop has depth 1 according to LLVM loop definition)
                //Surely, the most internal loop, the one with highest depth, is the one whose iterator is loopIteratorOp
                int dIt1 = ((LIT.getNestedLoops(it1)).second).size() + 1;
                int dIt2 = ((LIT.getNestedLoops(it2)).second).size() + 1;
                int dIt3 = ((LIT.getNestedLoops(it3)).second).size() + 1;
                int dIt4 = ((LIT.getNestedLoops(it4)).second).size() + 1;

                //Retrieving loop infos of each iterator
                LoopInfoTable::loopInfoStruct LISIt1 = LIT.getLoopInfo(it1);
                LoopInfoTable::loopInfoStruct LISIt2 = LIT.getLoopInfo(it2);
                LoopInfoTable::loopInfoStruct LISIt3 = LIT.getLoopInfo(it3);
                LoopInfoTable::loopInfoStruct LISIt4 = LIT.getLoopInfo(it4);


                if(dIt1 == 1 && dIt3 == 2 && dIt2 == 3 && dIt4 == 4){
                    //[i + k][j + l]
                    pointerInfo.rowFirst = true;
                    pointerInfo.colFirst = false;
                    pointerInfo.rowFirstSet = true;
                    pointerInfo.colFirstSet = false;

                    pointerInfo.setRows = LISIt2.iterations;
                    pointerInfo.setCols = LISIt4.iterations;

                }else if(dIt1 == 1 && dIt3 == 2 && dIt2 == 4 && dIt4 == 3){
                    //[i + l][j + k]
                    pointerInfo.rowFirst = true;
                    pointerInfo.colFirst = false;
                    pointerInfo.rowFirstSet = false;
                    pointerInfo.colFirstSet = true;

                    pointerInfo.setRows = LISIt4.iterations;
                    pointerInfo.setCols = LISIt2.iterations;

                }else if(dIt1 == 2 && dIt3 == 1 && dIt2 == 3 && dIt4 == 4){
                    //[j + k][i + l]
                    pointerInfo.rowFirst = false;
                    pointerInfo.colFirst = true;
                    pointerInfo.rowFirstSet = true;
                    pointerInfo.colFirstSet = false;

                    pointerInfo.setRows = LISIt2.iterations;
                    pointerInfo.setCols = LISIt4.iterations;

                }else if(dIt1 == 2 && dIt3 == 1 && dIt2 == 4 && dIt4 == 3){
                    //[j + l][i + k]
                    pointerInfo.rowFirst = false;
                    pointerInfo.colFirst = true;
                    pointerInfo.rowFirstSet = false;
                    pointerInfo.colFirstSet = true;

                    pointerInfo.setRows = LISIt4.iterations;
                    pointerInfo.setCols = LISIt4.iterations;

                }

                pointerInfo.numberOfSets = LISIt1.iterations * LISIt3.iterations;

            }

        }
    }else{
        //arrayOp refers to a 1-dim array

        if(!((firstIdxInfo.iterators).empty())){
            //If the index is composed of at least one iterator

            if((firstIdxInfo.iterators).size() == 1){
                //Index is composed by a single iterator

                //Retrieving the iterator
                int* firstIdxIt = (firstIdxInfo.iterators).front();
                //Retrieving loop infos of each iterator
                LoopInfoTable::loopInfoStruct LISIt1 = LIT.getLoopInfo(firstIdxIt);

                if(firstIdxIt == loopIteratorOp){
                    /*
                        for(firstIdx){
                            array[firstIdx]
                        }
                    */


                    pointerInfo.colFirst = false;
                    pointerInfo.rowFirst = true;
                    pointerInfo.setCols = LISIt1.iterations;
                    pointerInfo.setRows = arrayRows;
                    pointerInfo.numberOfSets = 1;
                }else if(firstIdxIt == higherLevelLoops.front()){
                    /*
                        for(firstIdx){
                            for(secondIdx){
                                array[firstIdx]
                            }
                        }
                    */
                    pointerInfo.colFirst = true;
                    pointerInfo.rowFirst = false;
                    pointerInfo.setCols = LISIt1.iterations;
                    pointerInfo.setRows = arrayRows;
                    pointerInfo.numberOfSets = 1;
                }
            }else if((firstIdxInfo.iterators).size() > 1){
                //Index is composed by a 2 iterators

                //The two array indexes are composed by two iterators each

                //Retrieving the iterators of the first index
                int* it1  = (firstIdxInfo.iterators).front(); //first iterator of the first index
                int* it2  = (firstIdxInfo.iterators).back(); //second iterator of the first index

                //If both operations between iterators of index are additions, i.e. [i + k][j + l]
                if(firstIdxInfo.operation == Instruction::Add){
                    
                    //Retrieving the depth of the loop associated to each iterator (Outermost loop has depth 1 according to LLVM loop definition)
                    //Surely, the most internal loop, the one with highest depth, is the one whose iterator is loopIteratorOp
                    int dIt1 = ((LIT.getNestedLoops(it1)).second).size() + 1;
                    int dIt2 = ((LIT.getNestedLoops(it2)).second).size() + 1;

                    //Retrieving loop infos of each iterator
                    LoopInfoTable::loopInfoStruct LISIt1 = LIT.getLoopInfo(it1);
                    LoopInfoTable::loopInfoStruct LISIt2 = LIT.getLoopInfo(it2);

                    if(dIt1 > dIt2){
                        /*
                            for(it2)
                                for(it1)
                                    array[it1 + it2]                    
                        */

                        pointerInfo.setRows = LISIt2.iterations;
                        pointerInfo.numberOfSets = LISIt1.iterations;

                    }else if(dIt1 < dIt2){
                        /*
                            for(it1)
                                for(it2)
                                    array[it1 + it2]                    
                        */

                        pointerInfo.setRows = LISIt1.iterations;
                        pointerInfo.numberOfSets = LISIt2.iterations;

                    }


                    pointerInfo.rowFirst = true;
                    pointerInfo.colFirst = false;
                    pointerInfo.rowFirstSet = false;
                    pointerInfo.colFirstSet = true;

                    pointerInfo.setCols = 1;

                }
            }
        }else{
            //The index is a constant
            pointerInfo.colFirst = false;
            pointerInfo.rowFirst = true;
            pointerInfo.setCols = 1;
            pointerInfo.setRows = 1;
            pointerInfo.numberOfSets = 1;
        }
    }

    //integer keeping track of the current index of the pointer
    int currIndex = 0;

    bool itMatch = false;

    //current iterator loop information
    LoopInfoTable::loopInfoStruct currLIS;

    AccessPattern accessPattern;
    AccessPattern accessPatternConstant;
    int index;

    for(auto idxInfoListIT = indexInfoList.begin(); idxInfoListIT != indexInfoList.end(); ++idxInfoListIT){
        index = 0;

        for(auto itVecIT = iterationVector.begin(); itVecIT != iterationVector.end(); ++itVecIT){
            //Cycling over the iterationVector list

            itMatch = false;
            for(auto itListIT = (idxInfoListIT->iterators).begin(); itListIT != (idxInfoListIT->iterators).end() && !itMatch; ++itListIT){
                //Cycling over all iterators forming the current index

                //Check if the current iterator of iterationVector is the same as the index iterator
                if(*itListIT == *itVecIT){
                    itMatch = true;

                    //Getting loop info of the loop whose iterator is pointed by itListIT
                    currLIS = LIT.getLoopInfo(*itListIT);
                }
            }
            //Filling the temp array access pattern structure
            if(itMatch){

                accessPattern.pushIncrement(currLIS.iteratorIncrement);

            }else{

                accessPattern.pushIncrement(0);

            }

            index++;

        }

        accessPatternConstant.pushIncrement(idxInfoListIT->constant);        

        currIndex++;
    }

    accessPattern.setRows(currIndex);
    accessPattern.setCols(index);
    accessPatternConstant.setRows(currIndex);
    accessPatternConstant.setCols(1);
    pointerInfo.pointerAccessPattern = accessPattern;
    pointerInfo.pointerAccessPatternConstant = accessPatternConstant;
    
    PIT.modifyPointerInfo(pointer, pointerInfo);
    //IMPLEMENT OTHER CASES

}

///Function useful to return aliasInfoMap
std::map<int*, std::list<int*>> * CollectInfo::getAliasInfo(){
    return &aliasInfoMap;
}

///Function useful to retrieve the list of BB effectively useful for scheduling
std::list<BasicBlock*> CollectInfo::getValidBBs(){
    return validBBs;
}

///Function that return the type of the instruction passed
CollectInfo::Instr CollectInfo::identifyInstr(Instruction &I){

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






void CollectInfo::printAliasInfoMap(){
    errs()<< "The state of the Alias Map will be printed:\n\n";

    for (aliasInfoMapIT = aliasInfoMap.begin(); aliasInfoMapIT != aliasInfoMap.end(); ++aliasInfoMapIT){
        
        errs()<< "allocatedReg: " << aliasInfoMapIT->first << "\n";
        for(auto listIT = (aliasInfoMapIT->second).begin(); listIT != (aliasInfoMapIT->second).end(); ++listIT){
            errs()<< "\tAliasReg: " << *listIT << "\n";
        }
        errs()<< "\n\n";
    }

    errs()<< "\n\n";
}

void CollectInfo::printArrayInfoMap(){
    errs()<< "The state of the pointerInfoMap will be printed:\n\n";

    for (arraysInfoMapIT = arraysInfoMap.begin(); arraysInfoMapIT != arraysInfoMap.end(); ++arraysInfoMapIT){
        
        errs()<< "allocatedReg related to array: " << arraysInfoMapIT->first << "\n";
        for(auto listIT = (arraysInfoMapIT->second).begin(); listIT != (arraysInfoMapIT->second).end(); ++listIT){
            errs()<< "\tArray size: " << *listIT << "\n";
        }
        errs()<< "\n\n";
    }

    errs()<< "\n\n";
}

void CollectInfo::printIteratorsAliasMap(){
    errs()<< "The state of the iteratorsAliasMap will be printed:\n\n";

    for (iteratorsAliasMapIT = iteratorsAliasMap.begin(); iteratorsAliasMapIT != iteratorsAliasMap.end(); ++iteratorsAliasMapIT){
        
        errs()<< "allocatedReg related to iterator: " << iteratorsAliasMapIT->first << "\n";
        errs()<< "\n";
    }

    errs()<< "\n\n";
}
