/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LiM Compiler Class: class useful for the generation of LiM array and FSM.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
// © Alessio Nicola 2021 (alessio.nicola@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

//Std Lib include files
#include <string>
#include <math.h>

//Octantis Include Files
#include "LiMCompiler.h"
#include "LiMArray.h"
#include "OperationsImplemented.h"

using namespace llvm;
using namespace octantis;

//NOTEs: The class has to be completely reconstructed (Especially for the management of arrays and loops)!


///Default constructor: it initializes the pointer to IT and the zeroAddr variable
LiMCompiler::LiMCompiler(InstructionTable & ptrIT):zeroAddr(0){

    //Flag for the identification of shift operations
//    bool isShiftLeft;
//    bool isShiftRight;

    //Variable to get the correct opcode (useful to identify the shift)
    //unsigned Opcode;

    //Starting address for loop allocation
    int * effAddrDestReg;

    //Iterator for the arrayNamesMap
    std::map<int*,std::list<int*>>::iterator internalANMIT;

    ptrInstrList = &(ptrIT.instructionList);

    errs() << "LiM Compiler, starting... ptrInstrList="<<ptrInstrList<<".\n\n";

    //Iteration over the instruction list
    for (instrListIT=(*ptrInstrList).begin(); instrListIT!=(*ptrInstrList).end(); ++instrListIT) {

        //Initialization of the isShifts Flag
//        isShiftLeft=false;
//        isShiftRight=false;

        //Number of memory rows that have to be placed inside the array to work
        //in parallel
        int numIter=instrListIT->arrayFactor;

        errs () << "\tEntering the reading loop...\n";

        //Identification of the kind of LiM row
        if((instrListIT->operation)=="load"){
            //Load operation
            errs()<<"\tRead load operation from IT. Type: ->" << (instrListIT->operation) << "<-\n";

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            int parallelism=32;
            std::string type="load";

            //The operation has to be performed considering the loop/array factor
            for (int i=0; i<numIter; ++i) {

                //get allocation address
                effAddrDestReg=getNewName();
                errs() << "Generated: " << effAddrDestReg << "\n";
                
                //Add new load instruction to the MemArray (CHECK IF NECESSARY ALL THESE REFERENCES!)
                MemArray.addNewRow(effAddrDestReg, type, parallelism); //The length of the LiM words has to be defined inside the
                                                                       //coniguration file.

                FSMLim.addNewInstruction(instrListIT->allocTime, effAddrDestReg);
                
                //Update the arrayMap
                addNewItem(instrListIT->destinationReg,effAddrDestReg);

            }

//            //Add new load instruction to the MemArray (CHECK IF NECESSARY ALL THESE REFERENCES!)
//            MemArray.addNewRow(instrListIT->destinationReg, type, parallelism); //The length of the LiM words has to be defined inside the
//                                                                                //coniguration file.


            //Check if shift operations are present
            if(!(instrListIT->specifications).empty())
            {
                //Find the element inside the arrayNamesMap
                internalANMIT=findInANM(instrListIT->destinationReg);

                std::list<std::string>::iterator specsIT;

                for(specsIT=(instrListIT->specifications).begin(); specsIT!=(instrListIT->specifications).end();++specsIT){

                    std::string spec=*specsIT;

                    //Cycle over all the elements of the array
                    for (std::list<int*>::iterator it=(internalANMIT->second).begin();
                         it!=(internalANMIT->second).end(); ++it) {
                        MemArray.addLogicToRow(*it, spec);
                    }

                    //MemArray.addLogicToRow(instrListIT->destinationReg, spec);
                }

            }

        } else if(LimOperations.find(instrListIT->operation)!= LimOperations.end()){
            //Generic operation
            errs()<<"\tRead a generic operation from IT. Type: ->" << (instrListIT->operation) << "<-\n";

            //Identification of particular instructions: the commented part
            // is more approprate for the identification of the information.
            // In the future updates implement it!

//            Opcode=(unsigned) instrListIT->operation;

//            if(Opcode==Instruction::Shl || Opcode==Instruction::Shr)
//            {
//                isShift=true;
//            }

//            if((instrListIT->operation)=="shl")
//            {
//                isShiftLeft=true;
//            } else if ((instrListIT->operation)=="shr"){
//                isShiftRight=true;
//            }

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            int parallelism=32;

            //Effective input for the destination register: the destination register
            //is a simple memory row in which saving the result of a computation,
            //provided by a LiM row.
            int * effInForDestReg;       

            //Identification of the kind of source operands
            bool isSrc1Array=false;
            bool isSrc2Array=false;

            //Variables to identify the source operands
            int * src1;
            int * src2;

            //Variable to identify the destination register
            //int * dest;

            //Iterator over the list inside the arrayNamesMap
            std::list<int *>::iterator regNameIT;

            //Identification if a reduction tree for an accumulation has to be performed
            bool isAccumulation;

            std::list<std::string>::iterator specsIT=find((instrListIT->specifications).begin(),
                                                          (instrListIT->specifications).end(),
                                                          "accumulation");
            if(specsIT!=(instrListIT->specifications).end())
            {
                isAccumulation=true;
                //Remove the label of accumulation inside the specs
                (instrListIT->specifications).erase(specsIT);
            } else {

                isAccumulation=false;
            }



            //Check if the operation is a shift
//            if(isShiftLeft || isShiftRight){

//                //Check if the source reg is of the same type
//                if(!MemArray.isLiMRowOfThisType(instrListIT->sourceReg1, instrListIT->operation)){

//                    //Update the logic inside the LiM row
//                    addLogicToRow(instrListIT->sourceReg1,instrListIT->operation);

//                }

//            }

            //Check the kinds of source operands that have to be considered (Single row / Array)
            if(numIter>1){
                //Find the Src1 row inside the arrayNamesMap
                internalANMIT=findInANM(instrListIT->sourceReg1);
                isSrc1Array= ((internalANMIT->second).size()>1) ? true : false;

                //Find the Src2 row inside the arrayNamesMap
                internalANMIT=findInANM(instrListIT->sourceReg2);
                isSrc2Array= ((internalANMIT->second).size()>1) ? true : false;
            }


            //Loop to allocate all the needed information inside the array
            //----------FROM HERE THE CODE IS UNDER MAINTENANCE----------//

            src1=instrListIT->sourceReg1;
            src2=instrListIT->sourceReg2;
            //dest=instrListIT->destinationReg;

            if(isAccumulation){

                errs() << "Starting the accumulation section...\n";

                //Size of the input array
                int arraySize;

                //Number of the elaboration steps
                int steps;

                //Temporary variable for storing the generated LiM row names
                int * tmpName;

                //Iterators to access the accumulationList
                std::list<int *>::iterator aListIT1;
                std::list<int *>::iterator aListIT2;

                //In case of accumulation the definition of the LiM Array is different
                if(isSrc1Array){

                    internalANMIT=findInANM(instrListIT->sourceReg1);

                } else if (isSrc2Array){

                    internalANMIT=findInANM(instrListIT->sourceReg2);

                } else {
                    //An error occurred
                    llvm_unreachable("Error in LiM Compiler: definition of an incorrect"
                                     "operation of accumulation.\n");
                }

                //Extraction of the elements that have to be accumulated
                for(std::list<int *>::iterator it=(internalANMIT->second).begin();
                    it!=(internalANMIT->second).end(); ++it) {

                    accumulationList.push_back(*it);

                }

                //Estraction of the needed information
                arraySize=accumulationList.size();
                steps=ceil(log2((double)arraySize))+2;

                //Loop for the allocation of the operations
                for (int i = 0; i < steps; ++i) {

                    aListIT1=accumulationList.begin();
                    aListIT2=std::next(aListIT1,1);

                    for (int j = 0; j < floor(arraySize/2); ++j) {
                        if(MemArray.changeLiMRowType(*aListIT2, instrListIT->operation, instrListIT->specifications)){
                                                //If the sourceReg2 is a normal memory row the function replaces its structure into LiM
                                                //whose type is defined by the operation that has to be performed.

                                                //Add new input connection for sourceReg1
                                                errs() << "\tChange LiM row performed!\n";
                                                MemArray.addNewInputConnection(*aListIT2, *aListIT1);

                                                //Update the available source register with the corresponding LiM structure (LIFO approach)
                                                effInForDestReg=*aListIT2;
                        } else {
                            //An error occurred
                            llvm_unreachable("Error in LiM Compiler: temporary LiM row not modifiable.\n");
                        }

                        //Generation of a new row
                        tmpName=getNewName();

                        //Function to add a new LiM Row inside the array: partial result row
                        MemArray.addNewResultRow(tmpName, parallelism, effInForDestReg);
                        FSMLim.addNewInstruction(instrListIT->allocTime, tmpName);

                        ////////////////DEBUG/////////////////////
                        errs()<<"Content of the accumulationList:\n";
                        for(std::list<int *>::iterator it=accumulationList.begin();
                            it!=accumulationList.end(); ++it){
                            errs()<< "\t" << *it << "\n";
                        }
                        errs() << "Elements pointed: " << *aListIT1 << " " << *aListIT2 <<"\n";
                        //////////////////////////////////////////

                        //Remove the two source LiM rows from the accumulationList
                        aListIT2=accumulationList.erase(aListIT2);
                        aListIT1=accumulationList.erase(aListIT1);

                        //Add the generated row
                        accumulationList.insert(aListIT1,tmpName);

                        //Updating the iterator to the next position
                        advance(aListIT2,1);

                        //Update the size of the accumulationList
                        arraySize=accumulationList.size();
                    }
                }

            } else {
                //Any other cases

                for (int loop = 0; loop < numIter; ++loop) {

                    //Assumption: if a vector is considered, at each iteration of the loop
                    //            a new index of the vector is taken into consideration.
                    //            So, possible operations are:
                    //              -> C[i]=B and A[i]
                    //              -> C[i]=B[i] and A[i]
                    if(isSrc1Array){
                         internalANMIT=findInANM(instrListIT->sourceReg1);
                         regNameIT=(internalANMIT->second).begin();
                         std::advance(regNameIT,loop);
                         src1=*regNameIT;
                    } else {
                        internalANMIT=findInANM(instrListIT->sourceReg1);
                        regNameIT=(internalANMIT->second).begin();
                        src1=*regNameIT;
                    }
                    if(isSrc2Array){
                        internalANMIT=findInANM(instrListIT->sourceReg2);
                        regNameIT=(internalANMIT->second).begin();
                        std::advance(regNameIT,loop);
                        src2=*regNameIT;
                    } else {
                        internalANMIT=findInANM(instrListIT->sourceReg2);
                        regNameIT=(internalANMIT->second).begin();
                        src2=*regNameIT;
                    }


                    if(MemArray.isLiMRowOfThisType(src2, instrListIT->operation)){
                        //Check if one of the two operands has the same LiM type

                        //sourceReg2 of the same type

                        //Add new input connection for sourceReg2
                        MemArray.addNewInputConnection(src2, src1);

                        //Update the effective in for the destination register:
                        effInForDestReg=src2;


                    } else if (MemArray.isLiMRowOfThisType(src1, instrListIT->operation)){
                        //sourceReg1 of the same type

                        //Add new input connection for sourceReg1
                        MemArray.addNewInputConnection(src1, src2);

                        //Update the effective in for the destination register:
                        effInForDestReg=src1;


                    } else if(MemArray.changeLiMRowType(src2, instrListIT->operation, instrListIT->specifications)){
                        //If the sourceReg2 is a normal memory row the function replaces its structure into LiM
                        //whose type is defined by the operation that has to be performed.

                        //Add new input connection for sourceReg1
                        errs() << "\tChange LiM row performed!\n";
                        MemArray.addNewInputConnection(src2, src1);

                        //Update the available source register with the corresponding LiM structure (LIFO approach)
                        effInForDestReg=src2;

                    } else if(MemArray.changeLiMRowType(src1, instrListIT->operation, instrListIT->specifications)){
                        //Add new input connection for sourceReg1
                        errs() << "\tChange LiM row performed!\n";
                        MemArray.addNewInputConnection(src1, src2);

                        //Update the available source register with the corresponding LiM structure (LIFO approach)
                        effInForDestReg=src1;

                    } else {
                        //It duplicates the original data into a new LiM row

                        //Firstly, a new Load instruction is inserted inside the Instruction Table
                        //to keep track of the time in which the allocation/copy is performed

                        //Iterator to find the position of the source register (SRC1) which will be copied
                        //      NOTEs: The procedure can be implemented directly inside the InstructionTable Class.
                        std::list<InstructionTable::instructionData>::iterator ptrInstrListTmp;
                        ptrInstrListTmp=ptrIT.getIteratorToElement(src1);

                        //Duplication of the Instruction Table row
                        InstructionTable::instructionData dataStruct=*ptrInstrListTmp;
                        dataStruct.destinationReg=getNewName();
                        effAddrDestReg=dataStruct.destinationReg;
                        
                        dataStruct.operation=(instrListIT->operation);

                        //Insertion of the source LiM Row before the result one
                        ptrIT.instructionList.insert(instrListIT,dataStruct);

                        //Then, a new instance of the data is added inside the map describing the
                        //LiM array: the new row is associated to the same key of the "parent" one
                        //inside the map

                        //Function to add a new LiM Row inside the array: partial result row
                        MemArray.addNewLiMRow(effAddrDestReg, instrListIT->operation, instrListIT->specifications,
                                              parallelism, src2);

    //                  //Function to add a new LiM Row inside the array: partial result row
    //                  MemArray.addNewLiMRow(instrListIT->destinationReg, instrListIT->operation, instrListIT->specifications,
    //                                          parallelism, instrListIT->sourceReg2);

                        //Update the address
                        //effAddrDestReg=getNewName();




                        ////////////DEBUG///////////
                        if(!(instrListIT->specifications).empty())
                        {
                            errs()<< "Lim compiler: additional logic list not empty!\n";
                        }
                        /////////END DEBUG//////////

                        //Allocate the instruction in time, inside the FSM
                        FSMLim.addNewInstruction(instrListIT->allocTime, effAddrDestReg);

                        //Define the effective input for destination register
                        effInForDestReg=effAddrDestReg; //Da verificare se è ok

                        //Advance the iterator by one position to come back to the result register
                        std::advance(instrListIT,1);
                    }

                    //The default value is associated to the declared one
                    //effAddrDestReg=instrListIT->destinationReg;

                    //errs() << "Generated: " << effAddrDestReg << "\n";

                    //Function to add a new LiM Row inside the array: partial result row
                    effAddrDestReg=getNewName();
                    MemArray.addNewResultRow(effAddrDestReg, parallelism, effInForDestReg);
                    FSMLim.addNewInstruction(instrListIT->allocTime, effAddrDestReg);
                    //Update the address
                    //effAddrDestReg=getNewName();

                }
            }

        } else {
            //An error occurred: the fetched instruction is not copatible with Octantis
            llvm_unreachable("LiMCompiler error: current instruction is uknown. Refer to "
                             "OperationsImplemented.h file.");

        }

    }
//-------------------------DEBUG SECTION----------------------

    printLiMArray();
    printFSM();


//---------------------END DEBUG SECTION----------------------

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
void LiMCompiler::addNewItem(int * const &origSrc, int * const &genName){
    arrayNamesMapIT=arrayNamesMap.find(origSrc);

    if(arrayNamesMapIT!=arrayNamesMap.end())
    {
        (arrayNamesMapIT->second).push_back(genName);
    } else {
        std::list<int *> tmpList = {genName};
        arrayNamesMap.insert({origSrc,tmpList});
    }
}

///Function to find an element inside the arrayNamesMap
std::map<int*,std::list<int*>>::iterator LiMCompiler::findInANM(int * const & srcReg){

    arrayNamesMapIT=arrayNamesMap.find(srcReg);
    if(arrayNamesMapIT!=arrayNamesMap.end()){
        return arrayNamesMapIT;
    } else {
        llvm_unreachable("Error in LiMCompiler: looking for a name not present inside the"
                         "arrayNamesMap.");
    }

}

///Function to get a new name for additional LiM rows
int * LiMCompiler::getNewName(){
    //Get the current name available
    int * returnValue=zeroAddr;
    //Update its value for the next cycle
    zeroAddr++;

    return returnValue;
}


//-------------------------DEBUG FUNCTIONS----------------------

void LiMCompiler::printLiMArray(){

    errs()<<"\n\nThe program has defined the LiMArray. In the following the structure of the"
            " map will be printed:\n\n";
    MemArray.printLiMArray();
    errs()<<"\nEnd of the content of the LiM Array.\n\n";

}
void LiMCompiler::printFSM(){

    errs()<<"The program has defined the FSM of the LiM Array. In the following the structure "
            "of the map will be printed:\n\n";
    FSMLim.printFSM();
    errs()<<"\nEnd of the content of the FSM.\n\n";


}

//---------------------END DEBUG FUNCTIONS----------------------
