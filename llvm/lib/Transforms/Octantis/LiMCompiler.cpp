/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LiM Compiler Class: class useful for the generation of a LiM object.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

//Std Lib include files
#include <string>

//Octantis Include Files
#include "LiMCompiler.h"
#include "LiMArray.h"
#include "OperationsImplemented.h"

using namespace llvm;
using namespace octantis;

//Default constructor: it initializes the pointer to IT and the constant variable zeroAddr
//      NOTEs: To verify the correct content of the zeroAddr variable.
LiMCompiler::LiMCompiler(InstructionTable & ptrIT):zeroAddr(((ptrIT.instructionList).begin())->destinationReg){

    ptrInstrList = &(ptrIT.instructionList);

    errs() << "LiM Compiler, starting... ptrInstrList="<<ptrInstrList<<".\n\n";

    //Iteration over the instruction list
    for (instrListIT=(*ptrInstrList).begin(); instrListIT!=(*ptrInstrList).end(); ++instrListIT) {

        errs () << "\tEntering the reading loop...\n";

        //Identification of the kind of LiM row
        if((instrListIT->operation)=="load"){
            //Load operation
            errs()<<"\tRead load operation from IT. Type: ->" << (instrListIT->operation) << "<-\n";

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            int parallelism=32;
            std::string type="norm";

            //Add new load instruction to the MemArray (CHECK IF NECESSARY ALL THESE REFERENCES!)
            MemArray.addNewRow(instrListIT->destinationReg, type, parallelism); //The length of the LiM words has to be defined inside the
                                                                                    //coniguration file.

        } else if(LimOperations.find(instrListIT->operation)!= LimOperations.end()){
            //Generic operation
            errs()<<"\tRead a generic operation from IT. Type: ->" << (instrListIT->operation) << "<-\n";

            //Temporary variable: It has to be removed considering
            //the info inside the config. file.
            int parallelism=32;

            //Effective input for the destination register: the destination register
            //is a simple memory row in which saving the result of a computation,
            //provided by a LiM row.
            int * effInForDestReg;

            //Check if one of the two operands has the same LiM type
            if(MemArray.isLiMRowOfThisType(instrListIT->sourceReg2, instrListIT->operation)){
                //sourceReg2 of the same type

                //Add new input connection for sourceReg2
                MemArray.addNewInputConnection(instrListIT->sourceReg2, instrListIT->sourceReg1);

                //Update the effective in for the destination register:
                effInForDestReg=instrListIT->sourceReg2;

            } else if (MemArray.isLiMRowOfThisType(instrListIT->sourceReg1, instrListIT->operation)){
                //sourceReg1 of the same type

                //Add new input connection for sourceReg1
                MemArray.addNewInputConnection(instrListIT->sourceReg1, instrListIT->sourceReg2 );

                //Update the effective in for the destination register:
                effInForDestReg=instrListIT->sourceReg1;

            } else if(MemArray.changeLiMRowType(instrListIT->sourceReg2, instrListIT->operation)){
                //If the sourceReg2 is a normal memory row the function replaces its structure into LiM
                //whose type is defined by the operation that has to be performed.

                //Add new input connection for sourceReg1
                errs() << "Change LiM row performed!\n";
                MemArray.addNewInputConnection(instrListIT->sourceReg2, instrListIT->sourceReg1);

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg=instrListIT->sourceReg2;

            } else if(MemArray.changeLiMRowType(instrListIT->sourceReg1, instrListIT->operation)){
                //Add new input connection for sourceReg1
                errs() << "Change LiM row performed!\n";
                MemArray.addNewInputConnection(instrListIT->sourceReg1, instrListIT->sourceReg2);

                //Update the available source register with the corresponding LiM structure (LIFO approach)
                effInForDestReg=instrListIT->sourceReg1;

            } else {
                //It duplicates the original data into a new LiM row

                //Firstly, a new Load instruction is inserted inside the Instruction Table
                //to keep track of the time in which the allocation/copy is performed

                //Iterator to find the position of the source register (SRC1) which will be copied
                //      NOTEs: The procedure can be implemented directly inside the InstructionTable Class.
                std::list<InstructionTable::instructionData>::iterator ptrInstrListTmp;
                ptrInstrListTmp=ptrIT.getIteratorToElement(instrListIT->sourceReg1);

                //Duplication of the Instruction Table row
                InstructionTable::instructionData dataStruct=*ptrInstrListTmp;
                dataStruct.destinationReg=zeroAddr;
                dataStruct.operation=(instrListIT->operation);

                //Update of the index of the extra LiM rows
                zeroAddr-=1;

                //Insertion of the source LiM Row before the result one
                ptrIT.instructionList.insert(instrListIT,dataStruct);

                //Then, a new instance of the data is added inside the map describing the
                //LiM array: the new row is associated to the same key of the "parent" one
                //inside the map
                MemArray.addNewLiMRow(instrListIT->destinationReg, instrListIT->operation, parallelism, instrListIT->sourceReg2);

                //Allocate the instruction in time, inside the FSM
                FSMLim.addNewInstruction(instrListIT->allocTime, instrListIT->destinationReg);

                //Define the effective destination register
                effInForDestReg=(instrListIT->destinationReg);

                //Advance the iterator by one position to come back to the result register
                std::advance(instrListIT,1);
            }

            //Function to add a new LiM Row inside the array: partial result row
             MemArray.addNewResultRow(instrListIT->destinationReg, parallelism, effInForDestReg);


        } else {
            //An error occurred: the fetched instruction is not copatible with Octantis
            llvm_unreachable("LiMCompiler error: current instruction is uknown. Refer to "
                             "OperationsImplemented.h file.");

        }

        //Allocate the instruction in time, inside the FSM
        FSMLim.addNewInstruction(instrListIT->allocTime, instrListIT->destinationReg);

    }
//-------------------------DEBUG SECTION----------------------

    printLiMArray();
    printFSM();


//---------------------END DEBUG SECTION----------------------

}

//-------------------------DEBUG FUNCTIONS----------------------

void LiMCompiler::printLiMArray(){

    errs()<<"The program has defined the LiMArray. In the following the structure of the"
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
