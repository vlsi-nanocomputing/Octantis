/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LiM Compiler Class: class useful for the generation of LiM array and FSM.
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
#include "LimCompilerNew.h"
#include "AccessPattern.h"
#include "LiMArray.h"
#include "OperationsImplemented.h"
#include "PointerInfoTable.h"
#include "LoopInfoTable.h"
#include "InstructionMap.h"

using namespace llvm;
using namespace octantis;

LimCompilerNew::LimCompilerNew(InstructionMap &instructionMap, CollectInfo &infoCollection):zeroAddr(0){

    //Assigning internal structures: the one with all information retrieved in the previous passes (infoCollection)
    //and the instructionMap, with the scheduled instructions
    InfoCollection = infoCollection;
    IM = instructionMap;

    //Iterator for the arrayNamesMap
    std::map<int*,std::list<int*>>::iterator internalANMIT;

    //scheduled instruction information
    std::string operation;
    std::list<std::string> specifications;
    int * destReg;
    int * destRegPtr;
    int * srcReg1;
    int * srcReg1Ptr;
    int * srcReg2;
    int * srcReg2Ptr;
    int ti;
    int constant;

    //In case an operand refers to an array/matrix, the relative information will be stored in these variables
    PointerInfoTable::pointerInfoStruct srcReg1PointerInfo;
    PointerInfoTable::pointerInfoStruct srcReg2PointerInfo;
    PointerInfoTable::pointerInfoStruct destRegPointerInfo;

    errs() << "LiM Compiler, starting...\n\n";

    //Iteration over the instruction map
    for (auto instructionOrderVectorIT = (IM.instructionOrderVector).begin(); instructionOrderVectorIT != (IM.instructionOrderVector).end(); ++instructionOrderVectorIT) {

        std::map<int*, InstructionMap::instructionData>::iterator instructionMapIT = (IM.instructionMap).find(instructionOrderVectorIT->first);

        //Assigning scheduling-relative information
        operation = (instructionMapIT->second).operation;
        specifications = (instructionMapIT->second).specifications;
        destReg = instructionMapIT->first;
        destRegPtr = (instructionMapIT->second).destRegPrt;
        srcReg1 = (instructionMapIT->second).sourceReg1;
        srcReg1Ptr = (instructionMapIT->second).srcReg1Ptr;
        srcReg2 = (instructionMapIT->second).sourceReg2;
        srcReg2Ptr = (instructionMapIT->second).srcReg2Ptr;
        ti = (instructionMapIT->second).ti;

        //Identification of the kind of LiM row
        if(operation == "load"){
            //Load operation
            errs()<<"\tRead load operation from IM. Type: ->" << operation << "<-\n";

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            std::string type = "load";
            int numIter;

            //Checking if the destination register is an array/matrix
            if(infoCollection.isArray(destReg)){
                //If it refers to an array, the number of iterations are retrieved by calling CollectInfo::getArraySize
                numIter = infoCollection.getArraySize(destReg);
            }else{
                numIter = 1;
            }

            //The operation has to be performed considering numIter
            for (int i = 0; i < numIter; ++i) {

                //get allocation address
                dest = getNewName();
                errs() << "\tGenerated address: " << dest << "\n";
                
                //Add new load instruction to the MemArray
                MemArray.addNewRow(dest, type, par);

                //Add new instruction to FSM cosidering ti from scheduling 
                FSMLim.addNewInstruction((instructionMapIT->second).ti, dest);
                
                //Update the arrayMap
                addNewItem(destReg, dest);

            }

            //Check if shift operations are present
            if(!(specifications.empty())){

                //Find the element inside the arrayNamesMap
                internalANMIT = findInANM(destReg);

                std::list<std::string>::iterator specsIT;

                for(specsIT = specifications.begin(); specsIT != specifications.end(); ++specsIT){

                    std::string spec=*specsIT;

                    //Cycle over all the elements of the array
                    for (std::list<int*>::iterator it = (internalANMIT->second).begin(); it != (internalANMIT->second).end(); ++it) {

                        MemArray.addLogicToRow(*it, spec);

                    }
                }

            }

        } else if(LimOperations.find(operation) != LimOperations.end()){
            //Generic operation
            errs()<<"\tRead a generic operation from IM. Type: ->" << operation << "<-\n";   

            //If true the operand is a 1-dimensional array, else a matrix
            bool hasSrc1OneDim = false;
            bool hasSrc2OneDim = false;
            bool hasDestOneDim = false;

            //If true the operand refers to a single variable
            bool isSrc1SingleVar = false;
            bool isSrc2SingleVar = false;
            bool isDestSingleVar = false;

            //If true, src2 is a constant
            bool isSrc2Constant = false;
            //If true, src1 is a constant
            bool isSrc1Constant = false;

            int * array;
            int arrayRows;
            int arrayCols;

            //Iterator over the list inside the arrayNamesMap
            std::list<int *>::iterator regNameIT;

            //Identification if a reduction tree for an accumulation has to be performed
            bool isAccumulation;

            std::list<std::string>::iterator specsIT = find(specifications.begin(), specifications.end(), "accumulation");
            if(specsIT != specifications.end())
            {
                isAccumulation = true;
                //Remove the label of accumulation inside the specs
                specifications.erase(specsIT);

            } else {

                isAccumulation=false;

            }


            //If an operand refers to an array, the info about the associated pointer must be retrieved
            //in order to know how to build the connections between rows
            if(srcReg1Ptr != nullptr){
                //srcReg1 is an array, check if it is a matrix or 1-dimensional array
                srcReg1PointerInfo = (infoCollection.PIT).getPointerInfo(srcReg1Ptr);

                if(((srcReg1PointerInfo.secondIdxInfo).iterators).empty()){
                    //If the second index iterators list is empty, the pointer refers to a 1-dimensional array
                    hasSrc1OneDim = true;
                }else{
                    hasSrc1OneDim = false;
                }
            }else if(ConstantInt *CI = dyn_cast<ConstantInt>((Value*)(srcReg1))){
                isSrc1Constant = true;
                constant = CI->getSExtValue();
            }else{
                isSrc1SingleVar = true; 
            }

            if(srcReg2Ptr != nullptr){
                //srcReg2 is an array, check if it is a matrix or 1-dimensional array
                srcReg2PointerInfo = (infoCollection.PIT).getPointerInfo(srcReg2Ptr);

                if(((srcReg2PointerInfo.secondIdxInfo).iterators).empty()){
                    //If the second index iterators list is empty, the pointer refers to a 1-dimensional array
                    hasSrc2OneDim = true;
                }else{
                    hasSrc2OneDim = false;
                }
            }else if(ConstantInt *CI = dyn_cast<ConstantInt>((Value*)(srcReg2))){
                isSrc2Constant = true;
                constant = CI->getSExtValue();
            }else{
                isSrc2SingleVar = true; 
            }

            if(destRegPtr != nullptr){
                //destReg is an array, check if it is a matrix or 1-dimensional array
                destRegPointerInfo = (infoCollection.PIT).getPointerInfo(destRegPtr);

                if(((destRegPointerInfo.secondIdxInfo).iterators).empty()){
                    //If the second index iterators list is empty, the pointer refers to a 1-dimensional array
                    hasDestOneDim = true;
                }else{
                    hasDestOneDim = false;
                }
            }else{
                isDestSingleVar = true;
            }

            if(isAccumulation){
                
                errs() << "Starting the accumulation section...\n";

                //Variable to store the information about the array/matrix on which an accumulation must be performed
                PointerInfoTable::pointerInfoStruct PointerInfo;
                //Iterator over the rows of the array
                std::list<int *>::iterator it;

                if(!isSrc1SingleVar){
                    //In this case, src1 is the array on which an accumulation must be performed,
                    //hence the iterator to its related rows is retrieved and also information about the array
                    //are gathered
                    internalANMIT = findInANM(srcReg1);

                    PointerInfo = (infoCollection.PIT).getPointerInfo(srcReg1Ptr);
                    array = (infoCollection.getAllocatedReg(srcReg1Ptr));
                    arrayRows = infoCollection.getArrayRows(array);
                    arrayCols = infoCollection.getArrayCols(array);

                }else{
                    //In general acc variable is src2

                    //If src1 is the accumulation variable, its row is deleted because a new result row,
                    //or more than one, will be inserted
                    arrayNamesMapIT = findInANM(srcReg1);
                    MemArray.deleteLiMRow((arrayNamesMapIT->second).front());
                    FSMLim.deleteInstruction((arrayNamesMapIT->second).front());
                    arrayNamesMap.erase(arrayNamesMapIT);

                } 
                
                if (!isSrc2SingleVar){
                    //In this case, src2 is the array on which an accumulation must be performed,
                    //hence the iterator to its related rows is retrieved and also information about the array
                    //are gathered
                    internalANMIT = findInANM(srcReg2);

                    PointerInfo = (infoCollection.PIT).getPointerInfo(srcReg2Ptr);
                    array = (infoCollection.getAllocatedReg(srcReg2Ptr));
                    arrayRows = infoCollection.getArrayRows(array);
                    arrayCols = infoCollection.getArrayCols(array);

                } else {
                    
                    //If src2 is the accumulation variable, its row is deleted because a new result row,
                    //or more than one, will be inserted
                    arrayNamesMapIT = findInANM(srcReg2);
                    MemArray.deleteLiMRow((arrayNamesMapIT->second).front());
                    FSMLim.deleteInstruction((arrayNamesMapIT->second).front());
                    arrayNamesMap.erase(arrayNamesMapIT);
                    zeroAddr--;

                }

                if(isSrc1SingleVar && isSrc2SingleVar){
                    PointerInfo.setRows = arrayRows;
                    PointerInfo.setCols = arrayCols;
                }

                //THIS IS ONLY FOR SETS WITH AN INCREMENT OF 1 (SUPERPOSITION OF 2)
                if(PointerInfo.setCols != -1){

                    //If the accumulation set sizes are defined and constant,
                    //for each set all accumulation elements are gathered according to the set size
                    //and pushed into the accumulation list that will be passed on to the function
                    //responsible fot the proper accumulation
                    for(int k = 0; k <= arrayRows - PointerInfo.setRows; ++k){
                        for(int l = 0; l <= arrayCols - PointerInfo.setCols; ++l){

                            for(int i = 0; i < PointerInfo.setRows; ++i){

                                it = (internalANMIT->second).begin();
                                //Start address of current set
                                std::advance(it , k * arrayCols + l);
                                //Start address of current set row
                                std::advance(it, i * arrayCols);

                                for(int j = 0; j < PointerInfo.setCols; ++j){
                                    
                                    if(j != 0){
                                        std::advance(it , 1);
                                    }
                                    //Pushing set data into accumulation list
                                    accumulationList.push_back(*it);
                                    errs() << "Inserting in acc list " << *it << "\n";

                                }

                                
                            }

                            

                            performAccumulationBinding(accumulationList, operation, specifications, srcReg1, destReg, ti); 
                            accumulationList.clear();
                        }
                    }           
                }else{

                    //If the accumulation set sizes are NOT defined and NOT constant,
                    //it means that a SAT-like algorithm takes place and set sizes change

                    //However, the accumulation elements are gathered and passed on to the
                    //accumulation-handling function as well as in the previous case

                    for(int k = 0; k <= arrayRows; ++k){
                        for(int l = 0; l <= arrayCols; ++l){

                            for(int i = 0; i < k; ++i){

                                it = (internalANMIT->second).begin();
                                //Start address of current set row
                                std::advance(it, i * arrayCols);

                                for(int j = 0; j < l; ++j){
                                    
                                    if(j != 0){
                                        std::advance(it , 1);
                                    }
                                    //Pushing set data into accumulation list
                                    accumulationList.push_back(*it);
                                    //errs() << "Inserting in acc list " << *it << "\n";

                                }

                                
                            }
                            performAccumulationBinding(accumulationList, operation, specifications, srcReg1, destReg, ti); 
                            errs() << "\n\n\n";
                            accumulationList.clear();
                        }
                    }  
                    errs() << "Finished\n";         
                }
                           

            } else {
                //Any other cases

                handleOperationBindingProcess(isSrc1SingleVar, isSrc2SingleVar, isDestSingleVar,
                                                hasSrc1OneDim, hasSrc2OneDim, hasDestOneDim,
                                                isSrc1Constant, isSrc2Constant,
                                                srcReg1, srcReg2, destReg, srcReg1Ptr, srcReg2Ptr, destRegPtr, constant,
                                                operation, specifications, ti,
                                                destRegPointerInfo, srcReg1PointerInfo, srcReg2PointerInfo);
   
            }
        }
    }

    //adjustResultRowAddress();

    FSMLim.transform();

    /////DEBUG/////
    printFSM();
    errs() << "\n\n\n\n";
    printLiMArray();
    errs() << "\n\n\n\n";
    FSMLim.printSelectionSignals();
    errs() << "\n\n\n\n";
    //printArrayNamesMap();
    //errs() << "\n\n\n\n";
    /////DEBUG/////

    /*std::list<int*> outputs = (arrayNamesMap.find(destReg))->second;
    int nOutputs = 0;
    for(auto listIT = outputs.begin(); listIT != outputs.end(); ++listIT){
        nOutputs++;
        errs() << *listIT << " is an output\n";

        int* newDest = getNewName();
        MemArray.addNewLiMRow(newDest, operation, specifications, par, *listIT, "OC");
        int time = FSMLim.getTime(*listIT) + 1;
        FSMLim.addNewInstruction(time, newDest);

    }
    MemArray.nOutputs = nOutputs;*/

}


void LimCompilerNew::performAccumulationBinding(std::list<int*> accumulationList, std::string operation,
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

    //Effective input for the destination register: the destination register
    //is a simple memory row in which saving the result of a computation,
    //provided by a LiM row.
    int * effInForDestReg;   

    //tmp list for the two src row to be inserted in addMap
    std::list<int*> operandList;

    //Estraction of the array size and the number of steps for the accumulation reduction to be performed
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

    
    bool isReduceable = true;
    std::list<int*> currList;
    std::copy(accumulationList.begin(), accumulationList.end(), std::back_inserter(currList));
    std::list<int*> nextList;
    std::list<int*> remainList;
    int time = 0;

    bool isMuxRow = false;
    int* muxRow;
    
    while(isReduceable && time < 7){

        isReduceable = false;
        arraySize = currList.size();
        for(int k = 0; k < arraySize; ++k){


            it1 = currList.begin();
            advance(it1, k);

            found = false;

            it2 = it1;

            //clock_t o = clock();

            for(int l = k + 1; (l < arraySize) && (!found); ++l){

                int adv = l - k;
                advance(it2, adv);

                int* alreadyAvailableResult = searchForResult(*it1, *it2);

                if(alreadyAvailableResult != nullptr){

                    std::string type = "load";
                    nextList.push_back(alreadyAvailableResult);

                    currList.remove(*it2);

                    found = true;                   
                }   

                it2 = it1;

            }

            //errs() << clock() - o << " o\n";


            if(!found){

                remainList.push_back(*it1);

            }else{

                isReduceable = true;

            }

            arraySize = currList.size();
        }

        accumulationListTime.insert(std::pair<int, std::list<int*>>(time, remainList));
        currList.clear();
        remainList.clear();
        std::copy(nextList.begin(), nextList.end(), std::back_inserter(currList));
        time++;
        nextList.clear();

    }
    

    if(isReduceable){

        accumulationListTime.insert(std::pair<int, std::list<int*>>(time, currList));

    }
    

    if(time == 0){
        accumulationListTime.insert(std::pair<int, std::list<int*>>(0, accumulationList));
        time++;
    }

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
        while(!loadRowsList.empty() || !addRowsList.empty()){

            if(tryLoad && !tryAdd && loadRowsListIT != loadRowsList.end() && !loadRowsList.empty()){

                currTimeAccList.push_back(*loadRowsListIT);
                loadRowsListIT = loadRowsList.erase(loadRowsListIT);
                tryAdd = true;

            }else{

                tryAdd = true;

            }
            
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
                //llvm_unreachable("Error in LiM Compiler: temporary LiM row not modifiable.\n");

                copyRowGenerated = true;

                //addMap.insert(std::pair<int*, std::list<int*>> (effAddrDestReg, operandList));

                //errs() << "Inserted in addMap " << effAddrDestReg << " " << operandList.front() << " " << operandList.back() << "\n";
                
            }

            if(!copyRowGenerated){


                //Generation of a new row
                tmpName=getNewName();

                addMap.insert(std::pair<int*, std::list<int*>> (tmpName, operandList));

                //errs() << "Inserted in addMap " << tmpName << " " << operandList.front() << " " << operandList.back() << "\n";

                operandList.clear();

                //Function to add a new LiM Row inside the array: partial result row
                MemArray.addNewResultRow(tmpName, par, effInForDestReg, "OLiM");

                int t1 = FSMLim.getTime(*aListIT1);
                int t2 = FSMLim.getTime(*aListIT2);
                int t = (t1 > t2) ? (t1 + 1) : (t2 + 1);


                /*if(!isMuxRow){

                    FSMLim.addNewInstruction(t, tmpName);
                    errs() << "\tAdded in FSM " << tmpName << " at time " << t << "\n";

                }else{

                    int tMux = t + 1;
                    FSMLim.addNewInstruction(tMux, tmpName);

                    //Adding control signals
                    std::list<int> tmpActiveTimeList = {t - 1, tMux - 1};
                    FSMLim.insertSelectionSignal(tmpActiveTimeList, muxRow);
                    errs() << "\tAdded in FSM " << tmpName << " at time " << tMux << "\n";
                    
                }*/


                errs() << "inserted " << tmpName << " at time " << t << "\n";
                FSMLim.addNewInstruction(t, tmpName);

                isMuxRow = false;
            }else{                

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
            //Update the arrayMap
            //errs() << "addNewItem " << destReg << " " << currTimeAccList.front() << "\n";
            addNewItem(destReg, currTimeAccList.front());
        }

    }

    if(steps == 0){
        //Update the arrayMap
        //errs() << "addNewItem " << destReg << " " << currTimeAccList.front() << "\n";
        addNewItem(destReg, currTimeAccList.front());
    }
}

int* LimCompilerNew::searchForResult(int* &srcRow1, int* &srcRow2){
    for(addMapIT = addMap.begin(); addMapIT != addMap.end(); ++addMapIT){
        if(((addMapIT->second).front() == srcRow1 || (addMapIT->second).back() == srcRow1) && 
        ((addMapIT->second).front() == srcRow2 || (addMapIT->second).back() == srcRow2)){
            return addMapIT->first;
        }
    }

    return nullptr;
}



///It handles the binding process for operations
void LimCompilerNew::handleOperationBindingProcess(const bool &isSrc1SingleVar, const bool &isSrc2SingleVar, const bool &isDestSingleVar,
                                const bool &hasSrc1OneDim, const bool &hasSrc2OneDim, const bool &hasDestOneDim,
                                const bool &isSrc1Constant, const bool &isSrc2Constant,
                                int* &srcReg1, int* &srcReg2, int* &destReg, int* &srcReg1Ptr, int* &srcReg2Ptr, int* &destRegPtr, const int &constant,
                                const std::string &operation, const std::list<std::string> &specifications, const int &ti,
                                PointerInfoTable::pointerInfoStruct destRegPointerInfo, const PointerInfoTable::pointerInfoStruct &srcReg1PointerInfo,
                                const PointerInfoTable::pointerInfoStruct &srcReg2PointerInfo){

                                    //Starting row address for dest reg
                                    int* tmpDestAddr = getNewName();

                                    //True if dest reg is an array declared in the C code
                                    bool isDestRealArray = false;
                                    
                                    if(isDestSingleVar){
                                        //If dest reg is a single variable and is is inside a loop
                                        if(!isSrc1SingleVar || !isSrc2SingleVar){

                                            int setColsSrc1 = (isSrc1SingleVar) ? 1 : srcReg1PointerInfo.setCols;
                                            int setRowsSrc1 = (isSrc1SingleVar) ? 1 : srcReg1PointerInfo.setRows;
                                            int setColsSrc2 = (isSrc2SingleVar) ? 1 : srcReg2PointerInfo.setCols;
                                            int setRowsSrc2 = (isSrc2SingleVar) ? 1 : srcReg2PointerInfo.setRows;

                                            //Insert infos about the tmp variable associated pointer, based on the one retrieved from the src reg
                                            destRegPointerInfo.numberOfSets = 1;

                                            destRegPointerInfo.setCols = (setColsSrc1 > setColsSrc2)
                                                                            ? setColsSrc1
                                                                            : setColsSrc2;
                                            destRegPointerInfo.setRows = (setRowsSrc1 > setRowsSrc2)
                                                                            ? setRowsSrc1
                                                                            : setRowsSrc2;                                            

                                            tmpVariableMap.insert(std::pair<int*, PointerInfoTable::pointerInfoStruct> (destReg, destRegPointerInfo));  
                                            errs() << destRegPointerInfo.setCols << " " <<destRegPointerInfo.setRows << "\n";
                                            for(int i = 0; i < destRegPointerInfo.setCols*destRegPointerInfo.setRows; ++i){
                                                addNewItem(destReg, tmpDestAddr);
                                                errs() << "Generated dest addr (tmp var) " << tmpDestAddr << "\n";

                                                if(i != destRegPointerInfo.setCols*destRegPointerInfo.setRows - 1){
                                                    tmpDestAddr = getNewName();
                                                }
                                                
                                            }

                                        }else{
                                            addNewItem(destReg, tmpDestAddr);
                                            destRegPointerInfo.setCols = 1;
                                            destRegPointerInfo.setRows = 1;
                                        }  
                                    }else{
                                        for(int i = 0; i < destRegPointerInfo.setCols*destRegPointerInfo.setRows; ++i){
                                            addNewItem(destReg, tmpDestAddr);
                                            errs() << "Generated dest addr " << tmpDestAddr << "\n";
                                            if(i != destRegPointerInfo.setCols*destRegPointerInfo.setRows - 1){
                                                tmpDestAddr = getNewName();
                                            }
                                        }

                                        isDestRealArray = true;
                                    }

                                    //Array access pattern for src1, src2 and dest
                                    //(valid if they are array or tmp variable involved in array caluclations)
                                    PointerInfoTable::arrayAccessPattern src1Pattern;
                                    PointerInfoTable::arrayAccessPattern src2Pattern;
                                    PointerInfoTable::arrayAccessPattern destPattern;

                                    src1Pattern = arrayAccessPatternHandler(srcReg1, srcReg1PointerInfo, isSrc1SingleVar, hasSrc1OneDim, isSrc1Constant);
                                    src2Pattern = arrayAccessPatternHandler(srcReg2, srcReg2PointerInfo, isSrc2SingleVar, hasSrc2OneDim, isSrc2Constant);
                                    destPattern = arrayAccessPatternHandler(destReg, destRegPointerInfo, isDestSingleVar, hasDestOneDim, false); 


                                    //Looping over all required source rows, first calculating the right address of source operand rows and
                                    //then issuing the proper binding                                    
                                    for(int i = 0; i < destRegPointerInfo.setCols*destRegPointerInfo.setRows; ++i){

                                        //Retrieving current address for src1 src2 and dest
                                        src1 = addressHandling(src1Pattern, srcReg1, srcReg1Ptr, srcReg1PointerInfo, isSrc1SingleVar, hasSrc1OneDim, isSrc1Constant, i);
                                        src2 = addressHandling(src2Pattern, srcReg2, srcReg2Ptr, srcReg2PointerInfo, isSrc2SingleVar, hasSrc2OneDim, isSrc2Constant, i);
                                        dest = addressHandling(destPattern, destReg, destRegPtr, destRegPointerInfo, isDestSingleVar, hasDestOneDim, false, i);

                                        errs() << "\t\tIn result generation loop iteration: " << i << "\n";
                                        errs() << "src1Addr: " << src1 << " src2Addr: " << src2 << " destAddr: " << dest << "\n";

                                        //Issuing the real binding
                                        realBinding(operation, specifications, ti, constant, isSrc2Constant, destReg, isDestRealArray);

                                    }

                                }



///It calculates the current address for the input reg based on input information
int* LimCompilerNew::addressHandling(PointerInfoTable::arrayAccessPattern patternType, int* reg, int* regPtr, const PointerInfoTable::pointerInfoStruct &regPointerInfo,
                                const bool &isRegSingleVar, const bool &hasRegOneDim, const bool &isRegConstant,
                                int i){


                                AccessPattern ap = regPointerInfo.pointerAccessPatternConstant;
                                std::vector<int> arrayAccessPatternConstant = ap.getIncrementVector();

                                std::list<int *>::iterator regStartAddr;
                                std::list<int *>::iterator regEndAddr;

                                //Finding the start address of reg
                                arrayNamesMapIT = findInANM(reg);
                                regStartAddr = (arrayNamesMapIT->second).begin();
                                //Finding the end address of reg
                                arrayNamesMapIT = findInANM(reg);
                                regEndAddr = (arrayNamesMapIT->second).end();

                                if(!isRegConstant && !isRegSingleVar){
                                    if(patternType != PointerInfoTable::undefined){
                                        //In case a pattern has been found

                                        int currRow, currCol;
                                        //int rows = regPointerInfo.setRows;
                                        int cols = InfoCollection.getArrayCols(InfoCollection.getAllocatedReg(regPtr));
                                        int setCols = regPointerInfo.setCols;
                                        errs() << cols << "\n";

                                        switch(patternType){
                                            //Switch among the patterns

                                            case PointerInfoTable::rowMajorPosColPos:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                    //Get increment of the iterator of the index
                                                    int idx1Increment = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorIncrement;
                                                    int idx1InitValue = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorInitValue;

                                                    currCol = i % setCols;

                                                    errs() << *regStartAddr << "\n";
                                                    errs() << currCol << "\n";
                                                    errs() << arrayAccessPatternConstant.front()*cols<< "\n";
                                                    errs() << idx1InitValue*cols << "\n";

                                                    if(currCol % idx1Increment == 0){
                                                        std::advance(regStartAddr, currCol + arrayAccessPatternConstant.front() + idx1InitValue);
                                                    }

                                                    return *regStartAddr;

                                                }else{
                                                    //If it is a matrix

                                                    //Get increments of the iterators relative to the indexes
                                                    int idx1Increment = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorIncrement;
                                                    int idx1InitValue = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorInitValue;
                                                    int idx2Increment = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).back())).iteratorIncrement;
                                                    int idx2InitValue = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).back())).iteratorInitValue;

                                                    currRow = i / setCols;
                                                    currCol = i % setCols;

                                                    if(currRow % idx1Increment == 0 && currCol % idx2Increment == 0){

                                                        std::advance(regStartAddr, currRow*cols + currCol +
                                                                                     arrayAccessPatternConstant.front()*cols + arrayAccessPatternConstant.back() +
                                                                                     idx1InitValue*cols + idx2InitValue);

                                                        errs() << *regStartAddr << "\n";                    
                                                    }

                                                    return *regStartAddr;

                                                }
                                            }
                                                break;

                                            case PointerInfoTable::rowMajorNegColPos:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                }else{
                                                    //If it is a matrix

                                                }
                                            }
                                                break;
                                        
                                            case PointerInfoTable::rowMajorPosColNeg:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                }else{
                                                    //If it is a matrix

                                                }
                                            }
                                                break;

                                            case PointerInfoTable::rowMajorNegColNeg:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                }else{
                                                    //If it is a matrix

                                                }
                                            }
                                                break;

                                            case PointerInfoTable::colMajorPosRowPos:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                    //Get increment of the iterator of the index
                                                    int idx1InitValue = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorInitValue;
                                                    int idx1Increment = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorIncrement;

                                                    currCol = i / setCols;

                                                    if(currCol % idx1Increment == 0){
                                                        std::advance(regStartAddr, currCol + arrayAccessPatternConstant.front() + idx1InitValue);
                                                    }

                                                    return *regStartAddr;

                                                }else{
                                                    //If it is a matrix

                                                    int idx1Increment = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorIncrement;
                                                    int idx2Increment = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).back())).iteratorIncrement;
                                                    int idx1InitValue = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).front())).iteratorInitValue;
                                                    int idx2InitValue = ((InfoCollection.LIT).getLoopInfo(((regPointerInfo.firstIdxInfo).iterators).back())).iteratorInitValue;

                                                    currRow = i % setCols;
                                                    currCol = i / setCols;

                                                    if(currRow % idx1Increment == 0 && currCol % idx2Increment == 0){
                                                        std::advance(regStartAddr, currCol + currRow*cols + 
                                                                                arrayAccessPatternConstant.front()*cols + arrayAccessPatternConstant.back() +
                                                                                idx1InitValue*cols + idx2InitValue);
                                                    }

                                                    return *regStartAddr;
                                                }
                                            }
                                                break;

                                            case PointerInfoTable::colMajorNegRowPos:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                }else{
                                                    //If it is a matrix

                                                }
                                            }
                                                break;

                                                case PointerInfoTable::colMajorPosRowNeg:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                }else{
                                                    //If it is a matrix

                                                }
                                            }
                                                break;

                                            case PointerInfoTable::colMajorNegRowNeg:
                                            {
                                                if(hasRegOneDim){
                                                    //If it is a 1-dim array

                                                }else{
                                                    //If it is a matrix

                                                }
                                            }
                                                break;

                                            default:
                                            {
                                                break;
                                            }
                                        }

                                    }else{
                                        
                                        errs() << "\nPATTERN IS UNDEFINED\n";

                                    }
                                }else{
                                    //reg is a single variable or costant
                                    if(isRegSingleVar){
                                        //reg is single var

                                        //Check if it is a tmp variable useful with multioperand operations
                                        tmpVariableMapIT = tmpVariableMap.find(reg);
                                        if(tmpVariableMapIT != tmpVariableMap.end()){

                                            std::advance(regStartAddr, i);
                                            return *regStartAddr;

                                        }else{
                                            
                                            return *regStartAddr;

                                        }   

                                    }else{  
                                        //reg is constant
                                    }
                                }
}

///It retrieves access pattern infos from PIT
PointerInfoTable::arrayAccessPattern LimCompilerNew::arrayAccessPatternHandler(int* reg, const PointerInfoTable::pointerInfoStruct &regPointerInfo,
                                const bool &isRegSingleVar, const bool &hasRegOneDim, const bool &isRegConstant){

                                //array access pattern for reg
                                PointerInfoTable::arrayAccessPattern patternType = PointerInfoTable::undefined;
                                
                                if(!isRegConstant && !isRegSingleVar){
                                    
                                    patternType = (InfoCollection.PIT).detectPointerAccessPattern(regPointerInfo.pointerAccessPattern);
                                
                                    return patternType;

                                }else{
                                    //reg refers to a single variable or constant

                                    //Check if reg is a tmp variable
                                    tmpVariableMapIT = tmpVariableMap.find(reg);
                                    if(tmpVariableMapIT != tmpVariableMap.end()){

                                        errs() << "Found tmp var!\n";
                                        return PointerInfoTable::rowMajorPosColPos;
                                        
                                    }
                                }

                                return PointerInfoTable::undefined;

}
  
///It carries out the real binding to LiM Rows
void LimCompilerNew::realBinding(std::string operation, std::list<std::string> specifications, const int &ti, const int &constant,
                                 const bool &isConstant, int* &destReg, const bool &isDestRealReg){

    //Effective input for the destination register: the destination register
    //is a simple memory row in which saving the result of a computation,
    //provided by a LiM row.
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

    std::map<int*, int*>::iterator lastDestMapIT1 = lastDestMap.find(src1);
    std::map<int*, int*>::iterator lastDestMapIT2 = lastDestMap.find(src2);

    if(lastDestMapIT1 != lastDestMap.end() && lastDestMapIT2 != lastDestMap.end() && lastDestMapIT1->second == lastDestMapIT2->second && !isDestRealReg){

        int* effInDest = (((MemArray.findRow(lastDestMapIT1->second))->second).inputConnections).front();
        std::string op = ((MemArray.findRow(effInDest))->second).rowType;

        if(op == operation){

            errs() << "\tThe two source operands already generated " << lastDestMapIT1->second << " with a " << operation <<
                        ", addresses will be adjusted to reuse that result\n";
            addNewItemInRefPos(destReg, lastDestMapIT1->second);
            adjustName();
            errs() << zeroAddr << "\n";

        }

    }else{
    
        if( !((operation == "sdiv" || operation == "mul" || operation == "shl" || operation == "shr" || operation == "ashr") && isConstant) ){
            //Check if one of the two operands has the same LiM type
            if(MemArray.isLiMRowOfThisType(src2, operation) && (((MemArray.findRow(src2))->second).inputConnections).size() < 3){

                //sourceReg2 of the same type

                //Add new input connection for sourceReg2
                MemArray.addNewInputConnection(src2, src1, "OC");
                errs() << "\t" << src2 << " is already of type " << operation << ", a new input connection is inserted, " << src1 << "\n";


                //FSMLim.insertSelectionSignal(,src2);

                //Update the effective in for the destination register:
                effInForDestReg = src2;

                //dest reg address
                effAddrDestReg = dest;

                isMuxRow = true;

                muxRow = src2;

            } else if (MemArray.isLiMRowOfThisType(src1, operation) && (((MemArray.findRow(src1))->second).inputConnections).size() < 3){
                //sourceReg1 of the same type

                //Add new input connection for sourceReg1
                MemArray.addNewInputConnection(src1, src2, "OC");
                errs() << "\t"<< src1 << " is already of type " << operation << ", a new input connection is inserted, " << src2 << "\n";

                //Update the effective in for the destination register:
                effInForDestReg = src1;

                //dest reg address
                effAddrDestReg = dest;

                isMuxRow = true;

                muxRow = src1;

            } else if(MemArray.changeLiMRowType(src2, operation, specifications)){
                //If the sourceReg2 is a normal memory row the function replaces its structure into LiM
                //whose type is defined by the operation that has to be performed.

                //Add new input connection for sourceReg1
                errs() << "\tChange LiM row " << src2 << " performed!\n";
                MemArray.addNewInputConnection(src2, src1, "OC");
                errs() << "\tNew input connection is inserted, " << src1 << "\n";

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg = src2;

                //dest reg address
                effAddrDestReg = dest;

            } else if(MemArray.changeLiMRowType(src1, operation, specifications)){

                //Add new input connection for sourceReg1
                errs() << "\tChange LiM row " << src1 << " performed!\n";
                MemArray.addNewInputConnection(src1, src2, "OC");
                errs() << "\tNew input connection is inserted, " << src2 << "\n";

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg = src1;

                //dest reg address
                effAddrDestReg = dest;

            } else {
                
                //int tiSrc1 = IM.insertNewInstructionDestReg(operation, getNewName(), src1);
        
                int *copyRow = getNewName();
                
                MemArray.addNewLiMRow(copyRow, operation, specifications, par, src1, "OC");
                
                MemArray.addNewInputConnection(copyRow, src2, "OC");

                //Allocate the instruction in time, inside the FSM
                int t1 = FSMLim.getTime(src1);
                int t2 = FSMLim.getTime(src2);
                tCopy = (t1 > t2) ? (t1 + 1) : (t2 + 1);
                FSMLim.addNewInstruction(tCopy, copyRow);

                errs() << "\tCopied " << src1 << " into copyRow " << copyRow << " ( " << tCopy << ") with input " << src2 << "\n";

                //Define the effective input for destination register
                effInForDestReg = copyRow; 

                effAddrDestReg = dest;

                copyRowGenerated = true;

            }

            if(!copyRowGenerated){

                //Function to add a new LiM Row inside the array: partial result row
                MemArray.addNewResultRow(effAddrDestReg, par, effInForDestReg, "OLiM");

                int t1 = FSMLim.getTime(src1);
                int t2 = FSMLim.getTime(src2);
                int t = (t1 > t2) ? (t1 + 1) : (t2 + 1);

                errs() << "\tTime src1 " << t1 << ", Time src2 " << t2 << "\n";
                if(!isMuxRow){

                    FSMLim.addNewInstruction(t, effAddrDestReg);
                    errs() << "\tAdded in FSM " << effAddrDestReg << " at time " << t << "\n";

                }else{

                    int tMux = t + 1;
                    FSMLim.addNewInstruction(tMux, effAddrDestReg);

                    //Adding control signals
                    std::list<int> tmpActiveTimeList = {t - 1, tMux - 1};
                    FSMLim.insertSelectionSignal(tmpActiveTimeList, muxRow);
                    errs() << "\tAdded in FSM " << effAddrDestReg << " at time " << tMux << "\n";
                    
                }


                lastDestMap.insert(std::pair<int*, int*> (src1, effAddrDestReg));
                lastDestMap.insert(std::pair<int*, int*> (src2, effAddrDestReg));

                isMuxRow = false;

            }else{                

                //Function to add a new LiM Row inside the array: partial result row
                MemArray.addNewResultRow(effAddrDestReg, par, effInForDestReg, "OLiM");

                tCopy = tCopy + 1;
                FSMLim.addNewInstruction(tCopy, effAddrDestReg);
                errs() << "\tDdded in FSM " << effAddrDestReg << " at time " << tCopy << "\n";

                copyRowGenerated = false;


                lastDestMap.insert(std::pair<int*, int*> (src1, effAddrDestReg));
                lastDestMap.insert(std::pair<int*, int*> (src2, effAddrDestReg));

            }

        }else{

            std::string loadType = "load";

            //If row is of type load, substitute type and add info about constant shift
            if(MemArray.isLiMRowOfThisType(src1, loadType)){

                std::list<std::string> specList;
                specList.push_back(std::to_string(constant));
                MemArray.changeLiMRowType(src1, operation, specList);

                //The input for src1 is src1 in order to shift it
                effInForDestReg = src1;
                effAddrDestReg = dest;

            }else{
                    
                effInForDestReg = getNewName();
                
                MemArray.addNewLiMRow(effInForDestReg, operation, specifications, par, src1, "OC");
                
                MemArray.addNewInputConnection(effInForDestReg, src2, "OC");

                copyRowGenerated = true;

            }


            int realConstant = floor(log2(constant));

            int tSrc1 = (!copyRowGenerated) ? FSMLim.getTime(src1) : tCopy;

            for(int i = tSrc1 + 1; i <= tSrc1 + realConstant + 1; ++i){
                if(i == tSrc1 + realConstant + 1){
                    FSMLim.addNewInstruction(i, effAddrDestReg);
                }else{
                    FSMLim.addNewInstruction(i, effInForDestReg);
                }
            }

        }
    }

}



// -- DEBUG FUNCTION -- //
// bool LiMCompiler::findValueInANM(int * const &srcReg)
// {
//     for (arrayNamesMapIT = arrayNamesMap.begin(); arrayNamesMapIT != arrayNamesMap.end(); ++arrayNamesMapIT)
//     {
//         errs()<<"first "<<arrayNamesMapIT->first<<"\n";
//         for (std::list<int *>::iterator it = (arrayNamesMapIT->second).begin(); it != (arrayNamesMapIT->second).end(); ++it)
//         {
//             errs()<<"second "<< *it <<"\n";
//             if (*it == srcReg)
//                 return true;
//         }
//     }
//     return false;
// }
// -- END DEBUG FUNCTION -- //

///Function to update the arrayNamesMap
void LimCompilerNew::addNewItem(int * const &origSrc, int * const &genName){
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
void LimCompilerNew::addNewItemInRefPos(int * const &origSrc, int * const &genName){
    arrayNamesMapIT = arrayNamesMap.find(origSrc);

    if(arrayNamesMapIT != arrayNamesMap.end()){

        auto listIT = std::find((arrayNamesMapIT->second).begin(), (arrayNamesMapIT->second).end(), dest);
        auto insertListIT = listIT++;
        (arrayNamesMapIT->second).insert(insertListIT, genName);
        (arrayNamesMapIT->second).remove(dest);

    }

}

///Function to find an element inside the arrayNamesMap
std::map<int*,std::list<int*>>::iterator LimCompilerNew::findInANM(int* srcReg){

    arrayNamesMapIT=arrayNamesMap.find(srcReg);
    if(arrayNamesMapIT!=arrayNamesMap.end()){
        return arrayNamesMapIT;
    } else {
        llvm_unreachable("Error in LiMCompiler: looking for a name not present inside the"
                         "arrayNamesMap.");
    }

}

///Function to get a new name for additional LiM rows
int * LimCompilerNew::getNewName(){
    //Get the current name available
    int * returnValue = zeroAddr;

    //Update its value for the next cycle
    zeroAddr++;

    return returnValue;
}

///Function to adjust current address if needed
void LimCompilerNew::adjustName(){
    
    zeroAddr--;

}


//-------------------------DEBUG FUNCTIONS----------------------

void LimCompilerNew::printLiMArray(){

    errs()<<"\n\nThe program has defined the LiMArray. In the following the structure of the"
            " map will be printed:\n\n";
    MemArray.printLiMArray();
    errs()<<"\nEnd of the content of the LiM Array.\n\n";

}

void LimCompilerNew::printFSM(){

    errs()<<"The program has defined the FSM of the LiM Array. In the following the structure "
            "of the map will be printed:\n\n";
    FSMLim.printFSM();
    errs()<<"\nEnd of the content of the FSM.\n\n";


}

void LimCompilerNew::printArrayNamesMap(){

    for(auto mapIT = arrayNamesMap.begin(); mapIT != arrayNamesMap.end(); ++mapIT){
        
        errs() << "reg " << mapIT->first << " is associated with the following rows\n";

        for(auto listIT = (mapIT->second).begin(); listIT != (mapIT->second).end(); ++listIT){
            errs() << "\t" << *listIT << "\n";
        }

    }

}

//---------------------END DEBUG FUNCTIONS----------------------
