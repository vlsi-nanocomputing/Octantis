/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LiM Compiler Class: class useful for the generation of a LiM object.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "LiMCompiler.h"
#include "LiMArray.h"
#include "OperationsImplemented.h"

//Std Lib include files
#include <string>

//Octantis Include Files


using namespace llvm;
using namespace octantis;

//Default constructor: it initializes the pointer to IT and the constant variable zeroAddr
//      NOTEs: To verify the correct content of the zeroAddr variable.
LiMCompiler::LiMCompiler(std::list<InstructionTable::instructionData> * ptrIL):zeroAddr(((*ptrInstrList).begin())->destinationReg){

    ptrInstrList = ptrIL;

    //Iteration over the instruction list
    for (instrListIT=(*ptrInstrList).begin(); instrListIT!=(*ptrInstrList).end(); ++instrListIT) {

        //Identification of the kind of LiM row
        if((instrListIT->operation)=="laod"){
            //Load operation

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            int parallelism=32;
            std::string type="norm";

            //Add new load instruction to the MemArray (CHECK IF NECESSARY ALL THESE REFERENCES!)
            MemArray.addNewRow(&(instrListIT->destinationReg), &type,&parallelism); //The length of the LiM words has to be defined inside the
                                                                                    //coniguration file.

        } else if(LimOperations.find(instrListIT->operation)!= LimOperations.end()){
            //Generic operation

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            int parallelism=32;

            //Effective input for the destination register: the destination register
            //is a simple memory row in which saving the result of a computation,
            //provided by a LiM row.
            int * effInForDestReg;

            //Check if one of the two operands has the same LiM type
            if(MemArray.isLiMRowOfThisType(&(instrListIT->sourceReg2),&(instrListIT->operation))){
                //sourceReg2 of the same type

                //Add new input connection
                MemArray.addNewInputConnection(&(instrListIT->sourceReg2),&(instrListIT->sourceReg1));

                //Update the effective in for the destination register:
                effInForDestReg=instrListIT->sourceReg2;

            } else if (MemArray.isLiMRowOfThisType(&(instrListIT->sourceReg1),&(instrListIT->operation))){
                //sourceReg1 of the same type

                //Add new input connection
                MemArray.addNewInputConnection(&(instrListIT->sourceReg1),&(instrListIT->sourceReg2));

                //Update the effective in for the destination register:
                effInForDestReg=instrListIT->sourceReg1;

            } else if(MemArray.changeLiMRowType(&(instrListIT->sourceReg2), &(instrListIT->operation))){

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg=instrListIT->sourceReg2;

            } else if(MemArray.changeLiMRowType(&(instrListIT->sourceReg1), &(instrListIT->operation))){
                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg=instrListIT->sourceReg1;

            } else {
                //It duplicates the original data into a new LiM row

                //Firstly, a new Load instruction is inserted inside the Instruction Table
                //to keep track of the time in which the allocation/copy is performed

                //Iterator to find the position of the source register (SRC1) which will be copied
                std::list<InstructionTable::instructionData>::iterator ptrInstrListTmp;
                ptrInstrListTmp=getIteratorToElement(instrListIT->sourceReg1,ptrIL);

                //Duplication of the Instruction Table row
                InstructionTable::instructionData dataStruct=*ptrInstrListTmp;
                dataStruct.destinationReg=zeroAddr;
                dataStruct.operation=(instrListIT->operation);

                //Update of the index of the extra LiM rows
                zeroAddr-=1;

                //Insertion of the source LiM Row before the result one
                (*ptrIL).insert(instrListIT,dataStruct);

                /*---Check if the src regs are the same of the copied LiM row or they have
                     to be modified in order to link them to the same copied row.---*/

                //Then, a new instance of the data is added inside the map describing the
                //LiM array: the new row is associated to the same key of the "parent" one
                //inside the map
                MemArray.addNewLiMRow(&(instrListIT->destinationReg), &(instrListIT->operation), &parallelism, &(instrListIT->sourceReg1)); //CRITICAL: CONTROL HOW TO MANAGE THE CONNECTIONS WITH PARENTS LIM CELLS!

                effInForDestReg=(instrListIT->destinationReg);

                //Advance the iterator by one position to come back to the result register
                std::advance(instrListIT,1);
            }

            //Add the new LiM row to perform the operation
            //MemArray.addNewLiMRow(&(instrListIT->destinationReg), &(instrListIT->operation), &parallelism, &(instrListIT->sourceReg1));

            //Check how to consider the different rows!!!

            //Function to add a new LiM Row inside the array: partial result row
             MemArray.addNewResultRow(&(instrListIT->destinationReg), &parallelism, &effInForDestReg);


        } else {
            //An error occurred: the fetched instruction is not copatible with Octantis
            llvm_unreachable("LiMCompiler error: current instruction is uknown. Refer to "
                             "OperationsImplemented.h file.");

        }
    }


}

//Function to find the position of an element inside the InstructionList
//(It may be moved inside the InstructionTable class!)
std::list<InstructionTable::instructionData>::iterator LiMCompiler::getIteratorToElement(int * position, std::list<InstructionTable::instructionData> * ptrIL){

    std::list<InstructionTable::instructionData>::iterator entryMatching;
    entryMatching=std::find_if((*ptrIL).begin(), (*ptrIL).end(), [position] (const InstructionTable::instructionData& iD) {
      return iD.destinationReg == position;
    });

    if (entryMatching != (*ptrIL).cend()) {

        return entryMatching;

    } else {
        //An error occurred: the parent register, the allocated one, is not present in IT
        llvm_unreachable("InstructionTable error: current instruction is not present"
                         " inside the instruction table");
    }

}
