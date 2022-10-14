/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LiMBinder Class: class useful for the generation of LiM array and FSM.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

//Std Lib include files
#include <string>
#include <math.h>

//Octantis Include Files
#include "LiMBinder.h"
#include "AccessPattern.h"
#include "LiMArray.h"
#include "OperationsImplemented.h"
#include "PointerInfoTable.h"
#include "LoopInfoTable.h"
#include "InstructionTable.h"

using namespace llvm;
using namespace octantis;

LiMBinder::LiMBinder(InstructionTable &instructionMap, CollectInfo &infoCollection, bool debugMode):zeroAddr(0), InfoCollection(infoCollection), IM(instructionMap), debugMode(debugMode){

    //Iterator for the arrayNamesMap
    std::map<int*,std::list<int*>>::iterator internalANMIT;

    errs() << "Starting the binding phase\n\n\n";

    //Iteration over instructionOrderVector in order to take into account instructions in order (load first, then all others)
    for (auto instructionOrderVectorIT = (IM.instructionOrderVector).begin(); instructionOrderVectorIT != (IM.instructionOrderVector).end(); ++instructionOrderVectorIT) {

        //Find the instruction in the instructionMap
        instructionMapIT = (IM.instructionMap).find(instructionOrderVectorIT->first);

        //Assigning scheduling-relative information
        operation = (instructionMapIT->second).operation;
        specifications = (instructionMapIT->second).specifications;
        destReg = instructionMapIT->first;
        destRegType = (instructionMapIT->second).destRegType;
        destRegPtr = (instructionMapIT->second).destRegPrt;
        srcReg1 = (instructionMapIT->second).sourceReg1;
        srcReg1Type = (instructionMapIT->second).srcReg1Type;
        srcReg1Ptr = (instructionMapIT->second).srcReg1Ptr;
        srcReg2 = (instructionMapIT->second).sourceReg2;
        srcReg2Type = (instructionMapIT->second).srcReg2Type;
        srcReg2Ptr = (instructionMapIT->second).srcReg2Ptr;
        ti = (instructionMapIT->second).ti;

        //Identification of the kind of LiM row
        if(operation == "load"){
            
            handleLoadOperationBindingProcess();

        } else if(LimOperations.find(operation) != LimOperations.end()){

            //Generic operation
            errs()<<"\tRead a ->" << operation << "<- operation from Instruction Map\n";   

            int * array;
            int arrayRows;
            int arrayCols;

            //Iterator over the list inside the arrayNamesMap
            std::list<int *>::iterator regNameIT;

            //Check for accumulation
            if(find(specifications.begin(), specifications.end(), "accumulation") != specifications.end()){

                //Accumulation operation handling
                handleAccumulationBindingProcess();

            } else {

                //Fully Parallel operation handling
                handleFPOperationBindingProcess();
   
            }

            //Reinitialize bool variables
            isSrc1Constant = false;
            isSrc2Constant = false;
            isSrc1SingleVar = false;
            isSrc2SingleVar = false;
            isDestSingleVar = false;
            isSrc1TmpVar = false;
            isSrc2TmpVar = false;
            hasDestOneDim = false;
            hasSrc1OneDim = false;
            hasSrc2OneDim = false;

        }
    }

    if(debugMode){
    errs() << "\n\n\n\n";
    printLiMArray();
    FSMLim.transform();
    printFSM();
    errs() << "\n\n\n\n";
    FSMLim.printSelectionSignals();
    errs() << "\n\n\n\n";
    }

}

///It handles the binding process for load operations
void LiMBinder::handleLoadOperationBindingProcess(){

    //Load operation
    errs()<<"\tRead a ->load<- operation from Instruction Map\n";

    //Temporary variable: It has to be removed considering
    //the info inside the config. file.
    std::string type = "load";
    int numIter;

    //Checking if the destination register is an array/matrix
    if(InfoCollection.isArray(destReg)){
        //If it refers to an array, the number of iterations are retrieved by calling CollectInfo::getArraySize
        numIter = InfoCollection.getArraySize(destReg);
    }else{
        numIter = 1;
    }

    //The operation has to be performed considering numIter
    for (int i = 0; i < numIter; ++i) {

        //get allocation address
        dest = getNewName();

        if(debugMode){
        errs() << "\t\tGenerated address: " << dest << "\n";
        }
        
        //Add new load instruction to the MemArray
        MemArray.addNewRow(dest, type, par);

        //Add new instruction to FSM cosidering ti from scheduling 
        FSMLim.addNewInstruction((instructionMapIT->second).ti, dest);
        
        //Update the arrayMap
        addNewItem(destReg, dest);

    }

}

///It handles the binding process for accumulation operations
void LiMBinder::handleAccumulationBindingProcess(){

    errs() << "\n\n\t\tStarting the accumulation binding\n\n\n";

    /*

    //Source reg 2 is always destination where the accumulation result is put
    if(isSrc2SingleVar){
        //Src reg 2 is a single variable

        arrayNamesMapIT = findInANM(srcReg2);
        src2 = (arrayNamesMapIT->second).front();
        std::list<int*> tmpList;
        tmpList.push_back(src2);
        addrListSrc2.setAddressList(tmpList);

    }else{
        //If src reg 2 is an array, retrieve addressList of src2

        addrListSrc2.shapeAddressList(srcReg2PointerInfo);
        //addrListSrc2.expandAddressList();
    }

    if(debugMode){
    errs() << "\tAddress List for src2 (dest):\n";
    addrListSrc2.printAddressList();
    }

    //Source reg 1 is always the array on which the accumulation is performed
    bool isSrc1TmpVar = false;

    //Checking if src1 or src2 are tmp variables
    tmpVariableListIT = find(tmpVariableList.begin(), tmpVariableList.end(), srcReg1);
    if(tmpVariableListIT != tmpVariableList.end()){
        isSrc1TmpVar = true;
    }

    if(isSrc1TmpVar){

        arrayNamesMapIT = findInANM(srcReg1);
        addrListSrc1.setAddressList(arrayNamesMapIT->second);

    }else{

        //Retrieve addressList of src1
        addrListSrc1.shapeAddressList(srcReg1PointerInfo);

        std::list<int*> itList = InfoCollection.LIT.getIterationVector(srcReg1PointerInfo.loopIterator);
        std::list<int> iterationList;
        for(std::list<int*>::iterator itListIT = itList.begin(); itListIT != itList.end(); ++itListIT){
            iterationList.push_back(InfoCollection.LIT.getLoopInfo(*itListIT).iterations);
        }

        addrListSrc1.expandAddressList(srcReg1PointerInfo, iterationList);

    }
    
    
    if(debugMode){
    errs() << "\tAddress List for src1 (acc):\n";
    addrListSrc1.printAddressList();
    }


    //Starting the actual accumulation mapping

    std::list<int*> tmpAccList;

    //VEDERE BENE
    if((srcReg1PointerInfo.numberOfSubsets > 1 && isDestSingleVar)){
        tmpVariableList.push_back(destReg);
    }



    if(isSrc1TmpVar){

        if(isDestSingleVar){

            performAccBinding(addrListSrc1.getSubset(0, addrListSrc2.getAddrListSize()), operation, specifications, srcReg1, destReg, ti); 
                    
        }else{

            int subsetSize = addrListSrc1.getAddrListSize() / addrListSrc2.getAddrListSize();

            for(int i = 1; i <= addrListSrc1.getAddrListSize(); i++){
                
                tmpAccList.push_back(addrListSrc1(i - 1));

                if(i % subsetSize == 0){

                    performAccBinding(tmpAccList, operation, specifications, srcReg1, destReg, ti); 

                    tmpAccList.clear();

                }

            }
            
        }

    }else{

        for(int i = 0; i < srcReg1PointerInfo.numberOfSubsets; i++){
        
            tmpAccList = addrListSrc1.getSubset(i, srcReg1PointerInfo.numberOfSubsetsElements);

            tmpAccList.push_back(addrListSrc2(i));

            performAccBinding(tmpAccList, operation, specifications, srcReg1, destReg, ti); 
                
            //Clear the list for next accumulation
            accumulationList.clear();

        }

    }
    */

    std::list<int*> itListSrc1 = InfoCollection.LIT.getIterationVector(srcReg1PointerInfo.loopIterator);
    std::list<int*> itListSrc2 = InfoCollection.LIT.getIterationVector(srcReg2PointerInfo.loopIterator);

    std::list<int> iterationListSrc1;
    std::list<int> iterationListSrc2;
    
    //Getting the iteration list for each of the two src registers
    for(std::list<int*>::iterator itListIT = itListSrc1.begin(); itListIT != itListSrc1.end(); ++itListIT){
        iterationListSrc1.push_back(InfoCollection.LIT.getLoopInfo(*itListIT).iterations);
    }

    for(std::list<int*>::iterator itListIT = itListSrc2.begin(); itListIT != itListSrc2.end(); ++itListIT){
        iterationListSrc2.push_back(InfoCollection.LIT.getLoopInfo(*itListIT).iterations);
    }



    //Source reg 2 is always destination where the accumulation result is put
    if(srcReg2Type == InstructionTable::singleVariable || srcReg2Type == InstructionTable::fakeArray){
        //Src reg 2 is a single variable

        addrListSrc2.setAddressList(findInANM(srcReg2)->second);

    }else if(srcReg2Type == InstructionTable::array){
        //If src reg 2 is an array, retrieve addressList of src2

        srcReg2PointerInfo = InfoCollection.PIT.getPointerInfo(srcReg2Ptr);

        addrListSrc2.shapeAddressList(srcReg2PointerInfo);
        addrListSrc2.expandAddressList(srcReg2PointerInfo, iterationListSrc2);

    }

    if(debugMode){
    errs() << "\tAddress List for src2 (dest):\n";
    addrListSrc2.printAddressList();
    }



    //Source reg 1 is always the set of elements on which the accumulation is performed
    if(srcReg1Type == InstructionTable::array){
        //If src reg 1 is an array, retrieve addressList of src1

        srcReg1PointerInfo = InfoCollection.PIT.getPointerInfo(srcReg1Ptr);

        addrListSrc1.shapeAddressList(srcReg1PointerInfo);
        addrListSrc1.expandAddressList(srcReg1PointerInfo, iterationListSrc1);

    }else if(srcReg1Type == InstructionTable::fakeArray){

        arrayNamesMapIT = findInANM(srcReg1);
        addrListSrc1.setAddressList(arrayNamesMapIT->second);

    }

    if(debugMode){
    errs() << "\tAddress List for src1 (acc set):\n";
    addrListSrc1.printAddressList();
    }



    
    /*std::list<int*> tmpAccList;
    int subsetSize = addrListSrc1.getAddrListSize() / addrListSrc2.getAddrListSize();

    for(int i = 1; i <= addrListSrc1.getAddrListSize(); i++){
        
        tmpAccList.push_back(addrListSrc1(i - 1));

        if(i % subsetSize == 0){

            performAccBinding(tmpAccList, operation, specifications, srcReg1, destReg, ti); 

            tmpAccList.clear();

        }

    }*/

    std::list<int*> tmpAccList;
    if(destRegType == InstructionTable::fakeArray){

        for(int i = 0; i < srcReg1PointerInfo.numberOfSubsets; i++){
        
            tmpAccList = addrListSrc1.getSubset(i, srcReg1PointerInfo.numberOfSubsetsElements);

            tmpAccList.push_back(addrListSrc2(i));

            performAccBinding(tmpAccList, operation, specifications, srcReg1, destReg, ti); 
                
            //Clear the list for next accumulation
            accumulationList.clear();

        }

    }

}

///It handles the binding process for operations
void LiMBinder::handleFPOperationBindingProcess(){

    //Starting row address for dest reg
    int* tmpDestAddr = getNewName();   

    int size1, size2;

    addrListSrc1.setAddressList(findInANM(srcReg1)->second);
    addrListSrc2.setAddressList(findInANM(srcReg2)->second);

    if(srcReg1Type == InstructionTable::singleVariable){
        //Src1 is a real SV

        addrListSrc1.setAddressList(findInANM(srcReg1)->second);
        size1 = 1;

        if(debugMode){
        errs() << "\t\tsrc1 refers to a single variable which has address: " << src1 << "\n\n";
        }

    }else if(srcReg1Type == InstructionTable::array){
        //If src1 is not a tmp var, it is a real array and the address list is built by means of shapeAddressList and expandAddressList

        srcReg1PointerInfo = InfoCollection.PIT.getPointerInfo(srcReg1Ptr);

        addrListSrc1.shapeAddressList(srcReg1PointerInfo);
        addrListSrc1.printAddressList();

        //List of loop iterators of the loop nest structure in which the pointer is contained
        std::list<int*> itList = InfoCollection.LIT.getIterationVector(srcReg1PointerInfo.loopIterator);
        std::list<int> iterationList;
        //The number of iterations of each loop in the nest structure is put inside a list
        for(std::list<int*>::iterator itListIT = itList.begin(); itListIT != itList.end(); ++itListIT){
            iterationList.push_back(InfoCollection.LIT.getLoopInfo(*itListIT).iterations);
        }

        addrListSrc1.expandAddressList(srcReg1PointerInfo, iterationList);
        size1 = addrListSrc1.getAddrListSize();
        addrListSrc1.printAddressList();

        if(debugMode){
        errs() << "\t\tsrc1 refers to an array\n\n";
        }

    }else if(srcReg1Type == InstructionTable::fakeArray){
        //If src1 is a tmp va2, its address list is already ordered and available

        srcReg1PointerInfo = InfoCollection.PIT.getPointerInfo(srcReg1Ptr);

        addrListSrc1.setAddressList(findInANM(srcReg1)->second);
        size1 = addrListSrc1.getAddrListSize();

        if(debugMode){
        errs() << "\t\tsrc1 refers to a tmp var\n\n";
        }       

        if(debugMode){
        addrListSrc1.printAddressList();
        }

    }else if(srcReg1Type == InstructionTable::constant){
        //src1 is a constant

        src1 = getNewName();
        size1 = 1;

        if(debugMode){
        errs() << "\t\tsrc1 refers to a constant\n\n";
        }
    }


    if(srcReg2Type == InstructionTable::singleVariable){
        //Src2 is a real SV

        //arrayNamesMapIT = findInANM(srcReg2);
        //src2 = (arrayNamesMapIT->second).front();
        addrListSrc2.setAddressList(findInANM(srcReg2)->second);
        size2 = 1;

        if(debugMode){
        errs() << "\t\tsrc2 refers to a single variable which has address: " << src2 << "\n\n";
        }

    }else if(srcReg2Type == InstructionTable::array){
        //If src2 is not a tmp var, it is a real array and the address list is built by means of shapeAddressList and expandAddressList

        srcReg2PointerInfo = InfoCollection.PIT.getPointerInfo(srcReg2Ptr);

        addrListSrc2.shapeAddressList(srcReg2PointerInfo);
        addrListSrc2.printAddressList();

        std::list<int*> itList = InfoCollection.LIT.getIterationVector(srcReg2PointerInfo.loopIterator);
        std::list<int> iterationList;
        for(std::list<int*>::iterator itListIT = itList.begin(); itListIT != itList.end(); ++itListIT){
            iterationList.push_back(InfoCollection.LIT.getLoopInfo(*itListIT).iterations);
        }

        addrListSrc2.expandAddressList(srcReg2PointerInfo, iterationList);
        addrListSrc2.printAddressList();
        size2 = addrListSrc2.getAddrListSize();

        if(debugMode){
        errs() << "\t\tsrc2 refers to an array\n\n";
        addrListSrc2.printAddressList();
        }
        

    }else if (srcReg2Type == InstructionTable::fakeArray){

        srcReg2PointerInfo = InfoCollection.PIT.getPointerInfo(srcReg2Ptr);

        //If src1 is a tmp vam, its address list is already ordered and available
        addrListSrc2.setAddressList(findInANM(srcReg2)->second);
        size2 = addrListSrc2.getAddrListSize();

        if(debugMode){
        errs() << "\t\tsrc2 refers to a tmp var\n\n";
        addrListSrc2.printAddressList();
        }

    }else if (srcReg2Type == InstructionTable::constant){
        //src2 is a constant

        src2 = getNewName();
        size2 = 1;

        if(debugMode){
        errs() << "\t\tsrc2 refers to a constant\n\n";
        }
    }


    
    int size = (size1 >= size2) ? size1 : size2;

    for(int i = 0; i < size; ++i){

        //If src1 is a real array or tmp var address is -> addrListSrc1(i)
        //If src1 is a constant or a sv address is -> src1
        src1 = (size1 == 1) ?  addrListSrc1(0) : addrListSrc1(i);

        //If src2 is a real array or tmp var address is -> addrListSrc2(i)
        //If src2 is a constant or a sv address is -> src2
        src2 = (size2 == 1) ?  addrListSrc2(0) : addrListSrc2(i);
        
        addNewItem(destReg, tmpDestAddr);
        dest = tmpDestAddr;

        if(debugMode){
        errs() << "\n\n\t\t\tMapping a " << operation << " operation with src1: " << src1 << ", src2: " << src2 << ", dest: " << dest << "\n\n";
        }

        performBinding();

        tmpDestAddr = getNewName();

    }

    if(debugMode){
        errs() << "\t\tSource reg 1 is of type: " << srcReg1Type << ", source reg 2 is of type: " << srcReg2Type << " and dest reg is of type: " << destRegType << "\n"
                << "\t\t\tsrc1 : " << src1 << "\n"
                << "\t\t\tsrc2 : " << src2 << "\n"
                << "\t\t\tdest : " << dest << "\n";
    }

}


///Function useful to implement the binding over an accumulation set
void LiMBinder::performAccBinding(std::list<int*> accumulationList, std::string operation,
                                                     std::list<std::string> specifications, int * srcReg1, int * destReg, int ti){
    //Size of the input array
    int arraySize;

    //Number of the elaboration steps
    int steps;

    //Temporary variable for storing the generated LiM row names
    int * tmpName;

    //Iterators to access the accumulationList
    std::list<int *>::iterator aListIT1;
    std::list<int *>::iterator aListIT2;

    //Effective input for the destination register
    int * effInForDestReg;   

    //tmp list for the two src rows to be inserted in addMap
    std::list<int*> operandList;

    //Extraction of the array size and the number of steps for the accumulation reduction to be performed
    arraySize = accumulationList.size();
    steps = ceil(log2((double)arraySize));

    //map in which the first field represents the time and the second the list of rows active at that time in order to 
    //perform accumulation reduction
    std::map<int, std::list<int*>> accumulationListTime;

    //initializing two iterators to the beggining of the accumulation list
    auto it1 = accumulationList.begin();
    auto it2 = accumulationList.begin();

    //it indicates if a result is already available
    bool found = false;

    //tInit is initialized to ti from scheduling
    int tInit = ti;

    //Variable useful to know if a copy row has been generated due to the impossibility of changing a row in binding
    bool copyRowGenerated = false;

    //variable useful to know if the accumulation list can be reduced
    bool isReduceable = true;

    //initial acc list
    std::list<int*> currList;
    std::copy(accumulationList.begin(), accumulationList.end(), std::back_inserter(currList));

    //Acc list for next step
    std::list<int*> nextList;

    std::list<int*> remainList;

    int time = 0;

    bool isMuxRow = false;
    int* muxRow;
    
    //Cycle that goes on as long as the number of elements of acc set can be reduced
    //the limit on time could be set within the config file, it indicates the maximum depth of the already
    //generated results that can be reused

    //This cycle allows reducing the accumulation set leveraging already available result rows
    while(isReduceable && time < 3){

        isReduceable = false;

        //Size of current accumulation set
        arraySize = currList.size();

        //Cycling over the elements of the acc list
        for(int k = 0; k < arraySize; ++k){

            //iterator to first element of list
            it1 = currList.begin();

            //it1 is advanced
            advance(it1, k);

            found = false;

            it2 = it1;

            //Cycle over other elements of acc set in order to recognize if two rows have already been
            //mapped to an addition operation
            for(int l = k + 1; (l < arraySize) && (!found); ++l){

                int adv = l - k;
                advance(it2, adv);

                //Searching for already available result row
                int* alreadyAvailableResult = searchForResult(*it1, *it2);

                //If partial result found
                if(alreadyAvailableResult != nullptr){

                    //Push result in the updated acc list
                    nextList.push_back(alreadyAvailableResult);

                    //Remove row that generated that result
                    currList.remove(*it2);

                    found = true;                   
                }   

                //Update it2
                it2 = it1;

            }

            if(!found){

                //If it1 points to a row that is not used to generated an intermediate result,
                //it remains in the acc list
                remainList.push_back(*it1);

            }else{

                isReduceable = true;

            }

            //Update size of current acc list
            arraySize = currList.size();
        }

        //At the end of the for-cycle, remainList contains all rows whose associated operations must be executed at the current time
        accumulationListTime.insert(std::pair<int, std::list<int*>>(time, remainList));

        //Clear lists
        currList.clear();
        remainList.clear();

        //nextList contains rows to be considered for the next time interval and it is copied to currList
        std::copy(nextList.begin(), nextList.end(), std::back_inserter(currList));

        //Update time
        time++;

        //Clear nextList
        nextList.clear();

    }
    

    //The accumulation set has been reduced
    //It is inserted in accumulationListTime at t=time
    if(isReduceable){
        accumulationListTime.insert(std::pair<int, std::list<int*>>(time, currList));
    }
    

    //The accumulation set has not been reduced
    //The initial acc list is inserted in accumulationListTime at time=0, ready to be mapped with the reduction approach after in the code
    if(time == 0){
        accumulationListTime.insert(std::pair<int, std::list<int*>>(0, accumulationList));
        time++;
    }

    //Starting from t=time, until the maximum number of steps needed to reduce the acc set,
    //empty temp lists are inserted in accumulationListTime
    for(int i = time; i < steps; ++i){
        std::list<int*> tmpEmptyList;
        accumulationListTime.insert(std::pair<int, std::list<int*>>(i, tmpEmptyList));
    }


    //IF NO AARs FOUND, REORGANIZE ACCLIST IN ORDER TO OPTIMIZE NEXT SET    
    if(((accumulationListTime.find(0))->second).size() == accumulationList.size()){
        for(size_t i = 0; i < accumulationList.size(); ++i){
            if(i == 0 || i % 2 == 0){
                auto it = ((accumulationListTime.find(0))->second).begin();
                std::advance(it, i);
                ((accumulationListTime.find(0))->second).push_back(*it);
                ((accumulationListTime.find(0))->second).erase(it);
            }
        }
    }

    std::list<int*> currTimeAccList;

    //Loop for the allocation of the operations
    for (int i = 0; i < steps; ++i) {

        currTimeAccList.merge((accumulationListTime.find(i))->second);
        std::string addType = "add";
        std::list<int*> addRowsList;
        std::list<int*> loadRowsList;
        
        //TRY TO ALTERNATE LOAD AND ADD ROWS IN ORDER NOT TO NEED TO INSERT ANOTHER LIM ROW

        //Putting add-rows in addRowsList and load-rows in loadRowsList
        for(auto listIT = currTimeAccList.begin(); listIT != currTimeAccList.end(); listIT++){
            if(MemArray.isLiMRowOfThisType(*listIT, addType)){
                addRowsList.push_back(*listIT);
            }else{
                loadRowsList.push_back(*listIT);
            }
        }

        currTimeAccList.clear();
        std::list<int*>::iterator addRowsListIT = addRowsList.begin();
        std::list<int*>::iterator loadRowsListIT = loadRowsList.begin();
        bool tryLoad = true;
        bool tryAdd = false;

        //Cycling until two lists are empty, trying to alternate add-rows with load-rows 
        while(!loadRowsList.empty() || !addRowsList.empty()){

            //Putting a load-row in currTimeAccList
            if(tryLoad && !tryAdd && loadRowsListIT != loadRowsList.end() && !loadRowsList.empty()){

                currTimeAccList.push_back(*loadRowsListIT);
                loadRowsListIT = loadRowsList.erase(loadRowsListIT);
                tryAdd = true;

            }else{

                tryAdd = true;

            }
            
            //Putting an add-row in currTimeAccList
            if(tryAdd && addRowsListIT != addRowsList.end() && !addRowsList.empty()){

                currTimeAccList.push_back(*addRowsListIT);
                addRowsListIT = addRowsList.erase(addRowsListIT);
                tryLoad = true;
                tryAdd = false;

            }else{

                tryLoad = true;
                tryAdd = false;

            }
        }

        tInit++;

        arraySize = currTimeAccList.size();
        
        aListIT1 = currTimeAccList.begin();
        aListIT2 = std::next(aListIT1,1);

        int tCopy;

        //Cycle useful for the real mapping of operations in the current acc set
        for (int j = 0; j < floor(arraySize/2); ++j) {

            //errs() << "Elements pointed: " << *aListIT1 << " " << *aListIT2 << " " << arraySize << "\n";

            /*if(MemArray.isLiMRowOfThisType(src2, operation) && (((MemArray.findRow(src2))->second).inputConnections).size() < 3){

                //sourceReg2 of the same type

                //Add new input connection for sourceReg2
                MemArray.addNewInputConnection(src2, src1, "OC");
                errs() << "\t" << src2 << " is already of type " << operation << ", a new input connection is inserted, " << src1 << "\n";


                //FSMLim.insertSelectionSignal(,src2);

                //Update the effective in for the destination register:
                effInForDestReg = src2;

                isMuxRow = true;

                muxRow = src2;

            } else if (MemArray.isLiMRowOfThisType(src1, operation) && (((MemArray.findRow(src1))->second).inputConnections).size() < 3){
                //sourceReg1 of the same type

                //Add new input connection for sourceReg1
                MemArray.addNewInputConnection(src1, src2, "OC");
                errs() << "\t"<< src1 << " is already of type " << operation << ", a new input connection is inserted, " << src2 << "\n";

                //Update the effective in for the destination register:
                effInForDestReg = src1;

                isMuxRow = true;

                muxRow = src1;

            }else*/ if(MemArray.changeLiMRowType(*aListIT2, operation, specifications)){
                //If the sourceReg2 is a normal memory row the function replaces its structure into LiM
                //whose type is defined by the operation that has to be performed.

                //Add new input connection for sourceReg1
                MemArray.addNewInputConnection(*aListIT2, *aListIT1, "OC");

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg=*aListIT2;

                operandList.push_back(*aListIT1);
                operandList.push_back(*aListIT2);

            } else if(MemArray.changeLiMRowType(*aListIT1, operation, specifications)){
                
                //If the sourceReg1 is a normal memory row the function replaces its structure into LiM
                //whose type is defined by the operation that has to be performed.

                //Add new input connection for sourceReg1
                MemArray.addNewInputConnection(*aListIT1, *aListIT2, "OC");

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg=*aListIT1;
                
                operandList.push_back(*aListIT1);
                operandList.push_back(*aListIT2);
                
            } else {
                
                //Both src1 and src2 are add rows, so a new row is generated (copy of src2)
                
                int * effAddrDestReg = getNewName();

                //int tiSrc1 = IM.insertNewInstructionDestReg(operation, effAddrDestReg, srcReg1);
                
                MemArray.addNewLiMRow(effAddrDestReg, operation, specifications, par, *aListIT1, "OC");
                
                MemArray.addNewInputConnection(effAddrDestReg, *aListIT2, "OC");

                //errs() << "\tGenerated row " << effAddrDestReg << " with inputs " << *aListIT1 << " and " << *aListIT2 << "\n";

                //Allocate the instruction in time, inside the FSM
                int t1 = FSMLim.getTime(*aListIT1);
                int t2 = FSMLim.getTime(*aListIT2);
                tCopy = (t1 > t2) ? (t1 + 1) : (t2 + 1);
                
                FSMLim.addNewInstruction(tCopy, effAddrDestReg);

                //Define the effective input for destination register
                effInForDestReg = effAddrDestReg; 

                operandList.push_back(effAddrDestReg);
                operandList.push_back(*aListIT2);

                copyRowGenerated = true;

                //addMap.insert(std::pair<int*, std::list<int*>> (effAddrDestReg, operandList));

                //errs() << "Inserted in addMap " << effAddrDestReg << " " << operandList.front() << " " << operandList.back() << "\n";
                
            }

            if(!copyRowGenerated){
                //A copy row has not been generated

                //Generation of a new row
                tmpName=getNewName();

                addMap.insert(std::pair<int*, std::list<int*>> (tmpName, operandList));

                //errs() << "Inserted in addMap " << tmpName << " " << operandList.front() << " " << operandList.back() << "\n";

                operandList.clear();

                //Function to add a new LiM Row inside the array, partial result row
                MemArray.addNewResultRow(tmpName, par, effInForDestReg, "OLiM");

                int t1 = FSMLim.getTime(*aListIT1);
                int t2 = FSMLim.getTime(*aListIT2);
                int t = (t1 > t2) ? (t1 + 1) : (t2 + 1);


                /*if(!isMuxRow){

                    FSMLim.addNewInstruction(t, tmpName);
                    if(debugMode){
                    errs() << "\tAdded in FSM " << tmpName << " at time " << t << "\n";
                    }

                }else{

                    int tMux = t + 1;
                    FSMLim.addNewInstruction(tMux, tmpName);

                    //Adding control signals
                    std::list<int> tmpActiveTimeList = {t - 1, tMux - 1};
                    FSMLim.insertSelectionSignal(tmpActiveTimeList, muxRow);

                    if(debugMode){
                    errs() << "\tAdded in FSM " << tmpName << " at time " << tMux << "\n";
                    }
                    
                }*/

                if(debugMode){
                errs() << "inserted " << tmpName << " at time " << t << "\n";
                }

                FSMLim.addNewInstruction(t, tmpName);

                isMuxRow = false;
            }else{                
                //A copy row has been generated

                operandList.clear();

                //Generation of a new row
                tmpName=getNewName();

                //Function to add a new LiM Row inside the array: partial result row
                MemArray.addNewResultRow(tmpName, par, effInForDestReg, "OLiM");

                tCopy = tCopy + 1;
                FSMLim.addNewInstruction(tCopy, tmpName);

                copyRowGenerated = false;

            }

            //Remove the two source LiM rows from the accumulationList
            aListIT2=currTimeAccList.erase(aListIT2);
            aListIT1=currTimeAccList.erase(aListIT1);

            //Add the generated row
            currTimeAccList.insert(aListIT1,tmpName);

            //Updating the iterator to the next position
            advance(aListIT2, 1);
        
            //errs() << *aListIT1 << " " << *aListIT2 << "\n";

        }

        
        if(i == steps - 1){
            if(debugMode){
            //errs() << "addNewItem " << destReg << " " << currTimeAccList.front() << "\n";
            errs() << "DEST: " << currTimeAccList.front() << "\n";
            }

            addNewItem(destReg, currTimeAccList.front());
        }

    }

    if(steps == 0){
        //Update the arrayMap

        if(debugMode){
        //errs() << "addNewItem " << destReg << " " << currTimeAccList.front() << "\n";
        errs() << "DEST1: " << currTimeAccList.front() << "\n";
        }

        addNewItem(destReg, currTimeAccList.front());
    }
}


///It carries out the real binding to LiM Rows
void LiMBinder::performBinding(){

    //Effective input for the destination register: the destination register
    //is a simple memory row in which saving the result of a computation, provided by a LiM row.
    int * effInForDestReg;

    //Address of destination row
    int * effAddrDestReg;

    //active time for copy row if needed
    int tCopy;

    //If true, a copy of a row has been generated and inserted in the array
    bool copyRowGenerated = false;

    //If true, a copy of a new input connection leading to a mux insertion has been added
    bool isMuxRow = false;

    //Row where a mux has been inserted
    int* muxRow;

    //variables useful to check for already mapped operations
    std::map<int*, int*>::iterator lastDestMapIT1 = lastDestMap.find(src1);
    std::map<int*, int*>::iterator lastDestMapIT2 = lastDestMap.find(src2);

    //Check if the operation between the two source operands has already been mapped in the LiM Array to avoid area overhead
    if(lastDestMapIT1 != lastDestMap.end() && lastDestMapIT2 != lastDestMap.end() && lastDestMapIT1->second == lastDestMapIT2->second){

        //Retrieving the row whose output is given in input to the dest row
        int* effInDest = (((MemArray.findRow(lastDestMapIT1->second))->second).inputConnections).front();

        //Retrieving the operation of the LiM row whose output is given in input to the dest row
        std::string op = ((MemArray.findRow(effInDest))->second).rowType;

        //If the operation type is equal to the current one, it means that it has already been mapped in the LiM Array
        if(op == operation){

            if(debugMode){
            errs() << "\t\t\t\tThe two source operands already generated " << lastDestMapIT1->second << " with a " << operation << ", the result will be reused\n";
            }

            addNewItemInRefPos(destReg, lastDestMapIT1->second);
            adjustName();

        }

    }else{
    
        //Case in which no shift operations are required
        if(!(operation == "shl" || operation == "shr" || operation == "ashr")){


            if(MemArray.changeLiMRowType(src2, operation, specifications)){

                //If the src2 is a normal memory row the function replaces its structure into LiM
                //whose type is defined by the operation that has to be performed.

                if(debugMode){
                errs() << "\t\t\t\tChange LiM row " << src2 << " performed!\n";
                }

                //Add new input connection src2 row
                MemArray.addNewInputConnection(src2, src1, "OC");

                if(debugMode){
                errs() << "\t\t\t\tNew input connection is inserted, " << src1 << "\n";
                }

                //Update the effective input for the destination register
                effInForDestReg = src2;

                //dest reg address
                effAddrDestReg = dest;

            } else if(MemArray.changeLiMRowType(src1, operation, specifications)){

                if(debugMode){
                errs() << "\t\t\t\tChange LiM row " << src1 << " performed!\n";
                }

                //Add new input connection to src1 row
                MemArray.addNewInputConnection(src1, src2, "OC");

                if(debugMode){
                errs() << "\t\t\t\tNew input connection is inserted, " << src2 << "\n";
                }

                //Update the effective input for the destination register
                effInForDestReg = src1;

                //dest reg address
                effAddrDestReg = dest;

            } else if(MemArray.isLiMRowOfThisType(src2, operation) && (((MemArray.findRow(src2))->second).inputConnections).size() < 3){

                //sourceReg2 of the same type

                //Add new input connection for sourceReg2
                MemArray.addNewInputConnection(src2, src1, "OC");

                if(debugMode){
                errs() << "\t\t\t\t" << src2 << " is already of type " << operation << ", a new input connection is inserted, " << src1 << "\n";
                }

                //Update the effective input for the destination register
                effInForDestReg = src2;

                //dest reg address
                effAddrDestReg = dest;

                //A mux will be needed
                isMuxRow = true;

                //Update address of row that needs a mux
                muxRow = src2;

            } else if (MemArray.isLiMRowOfThisType(src1, operation) && (((MemArray.findRow(src1))->second).inputConnections).size() < 3){

                //sourceReg1 of the same type

                //Add new input connection for sourceReg1
                MemArray.addNewInputConnection(src1, src2, "OC");

                if(debugMode){
                errs() << "\t\t\t\t"<< src1 << " is already of type " << operation << ", a new input connection is inserted, " << src2 << "\n";
                }

                //Update the effective input for the destination register
                effInForDestReg = src1;

                //dest reg address
                effAddrDestReg = dest;

                //A mux will be needed
                isMuxRow = true;

                //Update address of row that needs a mux
                muxRow = src1;

            } else {

                //The two src regs are of a different type from the one required
        
                //Generating a new address for a row in the array
                int *copyRow = getNewName();

                //Insert a new row with src1 as input
                MemArray.addNewLiMRow(copyRow, operation, specifications, par, src1, "OC");
                
                //Connect src2 to new row
                MemArray.addNewInputConnection(copyRow, src2, "OC");

                if(debugMode){
                errs() << "\t\t\t\tCopied " << src1 << " into copyRow " << copyRow << " ( " << tCopy << ") with input " << src2 << "\n";
                }

                //Define the effective input for destination register
                effInForDestReg = copyRow; 

                //dest reg
                effAddrDestReg = dest;

                copyRowGenerated = true;

                //Allocate the instruction in time, inside the FSM
                int t1 = FSMLim.getTime(src1);
                int t2 = FSMLim.getTime(src2);
                tCopy = (t1 > t2) ? (t1 + 1) : (t2 + 1);
                FSMLim.addNewInstruction(tCopy, copyRow);

            }

            if(!copyRowGenerated){
                //If a copy row has not been generated

                //Function to add a new LiM Row inside the array, the partial result row
                MemArray.addNewResultRow(effAddrDestReg, par, effInForDestReg, "OLiM");

                //Time t is the one in which the instruction can be executed
                int t1 = FSMLim.getTime(src1);
                int t2 = FSMLim.getTime(src2);
                int t = (t1 > t2) ? (t1 + 1) : (t2 + 1);

                if(debugMode){
                errs() << "\t\t\t\tTime src1 " << t1 << ", Time src2 " << t2 << "\n";
                }

                if(!isMuxRow){

                    //If a mux is not needed, the instruction is simply inserted in the FSM
                    FSMLim.addNewInstruction(t, effAddrDestReg);

                    if(debugMode){
                    errs() << "\t\t\t\tAdded in FSM " << effAddrDestReg << " at time " << t << "\n";
                    }

                }else{

                    //If a mux is needed, t must be incremented by one
                    int tMux = t + 1;
                    FSMLim.addNewInstruction(tMux, effAddrDestReg);

                    //Adding control signals
                    std::list<int> tmpActiveTimeList = {t - 1, tMux - 1};
                    FSMLim.insertSelectionSignal(tmpActiveTimeList, muxRow);

                    if(debugMode){
                    errs() << "\t\t\t\tAdded in FSM " << effAddrDestReg << " at time " << tMux << "\n";
                    }
                    
                }

                //Update lastDestMap
                lastDestMap.insert(std::pair<int*, int*> (src1, effAddrDestReg));
                lastDestMap.insert(std::pair<int*, int*> (src2, effAddrDestReg));

                //Reset isMuxRow
                isMuxRow = false;

            }else{                
                //Copy row has been inserted

                //Function to add a new LiM Row inside the array, partial result row
                MemArray.addNewResultRow(effAddrDestReg, par, effInForDestReg, "OLiM");

                //Time for dest reg
                tCopy = tCopy + 1;
                FSMLim.addNewInstruction(tCopy, effAddrDestReg);

                if(debugMode){
                errs() << "\t\t\t\tAdded in FSM " << effAddrDestReg << " at time " << tCopy << "\n";
                }

                //Reset copyRowGenerated
                copyRowGenerated = false;

                //Update lastDestMap
                lastDestMap.insert(std::pair<int*, int*> (src1, effAddrDestReg));
                lastDestMap.insert(std::pair<int*, int*> (src2, effAddrDestReg));

            }

        }else{

            //Case involving shift operations
            
            std::string loadType = "load";

            //If row is of type load, substitute type and add info about constant shift
            if(MemArray.isLiMRowOfThisType(src1, loadType)){

                std::list<std::string> specList;
                specList.push_back(std::to_string(constant));
                MemArray.changeLiMRowType(src1, operation, specList);

                //Add partial result row
                MemArray.addNewResultRow(dest, par, src1, "OC");

                //The input for src1 is src1 in order to shift it
                effInForDestReg = src1;
                effAddrDestReg = dest;

            }else{

                //METTERE A POSTO
                //Copy row needed
                effInForDestReg = getNewName();
                
                MemArray.addNewLiMRow(effInForDestReg, operation, specifications, par, src1, "OC");
                
                MemArray.addNewInputConnection(effInForDestReg, src2, "OC");

                copyRowGenerated = true;

            }

            
            //Insert info in FSM in order to handle the constant shift

            int realConstant = floor(log2(constant));

            int tSrc1 = (!copyRowGenerated) ? FSMLim.getTime(src1) : tCopy;

            if(debugMode){
            //errs() << "tSRC1: " << tSrc1 << ". realConstant: " << realConstant << "\n";
            }

            for(int i = tSrc1 + 1; i <= tSrc1 + realConstant + 1; ++i){
                if(i == tSrc1 + realConstant + 1){

                    if(debugMode){
                    //errs() << "Added: " << effAddrDestReg << " at time:" << i << "\n";
                    }

                    FSMLim.addNewInstruction(i, effAddrDestReg);
                }else{

                    if(debugMode){
                    //errs() << "Added: " << effInForDestReg << " at time:" << i << "\n";
                    }

                    FSMLim.addNewInstruction(i, effInForDestReg);
                }
            }
            

        }
    }

}






///It searches for an already available result in accumulations
int* LiMBinder::searchForResult(int* &srcRow1, int* &srcRow2){
    for(addMapIT = addMap.begin(); addMapIT != addMap.end(); ++addMapIT){
        if(((addMapIT->second).front() == srcRow1 || (addMapIT->second).back() == srcRow1) && 
        ((addMapIT->second).front() == srcRow2 || (addMapIT->second).back() == srcRow2)){
            return addMapIT->first;
        }
    }

    return nullptr;
}

///Function to update the arrayNamesMap
void LiMBinder::addNewItem(int * const &origSrc, int * const &genName){
    arrayNamesMapIT=arrayNamesMap.find(origSrc);

    if(arrayNamesMapIT!=arrayNamesMap.end())
    {
        (arrayNamesMapIT->second).push_back(genName);
    } else {
        std::list<int *> tmpList = {genName};
        arrayNamesMap.insert({origSrc,tmpList});
    }
}

///Function to update the arrayNamesMap
void LiMBinder::addNewItemInRefPos(int * const &origSrc, int * const &genName){
    arrayNamesMapIT = arrayNamesMap.find(origSrc);

    if(arrayNamesMapIT != arrayNamesMap.end()){

        auto listIT = std::find((arrayNamesMapIT->second).begin(), (arrayNamesMapIT->second).end(), dest);
        auto insertListIT = listIT++;
        (arrayNamesMapIT->second).insert(insertListIT, genName);
        (arrayNamesMapIT->second).remove(dest);

    }

}

///Function to find an element inside the arrayNamesMap
std::map<int*,std::list<int*>>::iterator LiMBinder::findInANM(int* srcReg){

    arrayNamesMapIT=arrayNamesMap.find(srcReg);
    if(arrayNamesMapIT!=arrayNamesMap.end()){
        return arrayNamesMapIT;
    } else {
        llvm_unreachable("Error in LiMCompiler: looking for a name not present inside the"
                         "arrayNamesMap.");
    }

}

///Function to get a new name for additional LiM rows
int * LiMBinder::getNewName(){
    //Get the current name available
    int * returnValue = zeroAddr;

    //Update its value for the next cycle
    zeroAddr++;

    return returnValue;
}

///Function to adjust current address if needed
void LiMBinder::adjustName(){
    
    zeroAddr--;

}


//-------------------------DEBUG FUNCTIONS----------------------

void LiMBinder::printLiMArray(){

    errs()<<"\n\nThe program has defined the LiMArray. In the following the structure of the"
            " map will be printed:\n\n";
    MemArray.printLiMArray();
    errs()<<"\nEnd of the content of the LiM Array.\n\n";

}

void LiMBinder::printFSM(){

    errs()<<"The program has defined the FSM of the LiM Array. In the following the structure "
            "of the map will be printed:\n\n";
    FSMLim.printFSM();
    errs()<<"\nEnd of the content of the FSM.\n\n";


}

void LiMBinder::printArrayNamesMap(){

    for(auto mapIT = arrayNamesMap.begin(); mapIT != arrayNamesMap.end(); ++mapIT){
        
        errs() << "reg " << mapIT->first << " is associated with the following rows\n";

        for(auto listIT = (mapIT->second).begin(); listIT != (mapIT->second).end(); ++listIT){
            errs() << "\t" << *listIT << "\n";
        }

    }

}

//---------------------END DEBUG FUNCTIONS----------------------
