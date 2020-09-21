/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PrintDexFile Class: class useful for the definition of Dexima's configuration file (.dex).
//      *Note: For the syntax description, refer to Dexima's documentation.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "PrintDexFile.h"

// Standard C++ Include Files
#include <cmath>
#include <iostream>
#include <fstream>
#include <system_error>

//Debug
#include <stdexcept>

//LLVM include files
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ErrorHandling.h"

using namespace octantis;
using namespace llvm;

/// It prints the .dex file
void PrintDexFile::print(){

    //Definition of the Output file
//    std::error_code ErrorInfo;
//    std::string outFileName = "config.dex";
//    raw_fd_ostream outFile(outFileName.c_str(), ErrorInfo, llvm::sys::fs::OF_None);

//    if (ErrorInfo) {
//        errs() << "Error during the opening of the output file: " << ErrorInfo.message() << '\n';
//        llvm_unreachable("Error in PrintDexFile: unable to create the .dex output file.");
//    }

    //////////////ONLY FOR DEBUG PURPOSES///////////////
    std::string outFileName_scheleton = "config.dex";
    std::string outFileName_LiMMap = "config_Map.dex";
    Output.open(outFileName_scheleton,std::fstream::out);
    OutputLiMMap.open(outFileName_LiMMap,std::fstream::out);
    ////////////////////////////////////////////////////

    //Printing the content of the different sections:
      printConstants();
      printInit();
      printLim();
//    printMap();//Outside logic, not implemented
    //printInstructions();

    //Add the remaining sections!

    //Printing the results of the synthesis process
   // outFile << Output.str();
//    outFile.close();

      //////////////ONLY FOR DEBUG PURPOSES///////////////
      Output.close();
      OutputLiMMap.close();
      ////////////////////////////////////////////////////

    errs() << "Octantis finished the synthesis process with NO ERROR. Goodbye!\n\n";
}

/// It prints the Constants section
void PrintDexFile::printConstants(){
    Output << "begin constants\n\n";
    // Printing the CONSTANTS section of Dexima's conf.file

    //The section should be implemented as a future works

    Output << "end constants\n\n";
}

/// It prints the Init section
void PrintDexFile::printInit(){
    Output << "begin init\n\n";
    // Printing the INIT section of Dexima's conf.file


    //Also here the parallelsm of the memory has to be defined
    //into a configuration file.
    Output << "LIM Lim(" << floor(log2((*compArray).getDimensions()))
           << ", 32)\n";

    Output << "end init\n\n";
}

/// It prints the Lim section
void PrintDexFile::printLim(){

    //Variable to know the current row analyzed
    int currentRow=0;


    Output << "begin Lim\n\n";
    // Printing the LIM section of Dexima's conf.file

    //MEMDEF REGION//////////////////////////////////////////
    Output << "\tbegin memdef\n\n";

    //Also here the parallelsm of the memory has to be defined
    //into a configuration file.
    Output << "\t\tROWS " << (*compArray).getDimensions() << "\n"
           << "\t\tCOLUMNS 32\n"
           << "\t\tTYPE SRAM\n";

    Output << "\tend memdef\n\n";
    //END MEMDEF REGION//////////////////////////////////////


    //LOGIC REGION///////////////////////////////////////////
    Output << "\tbegin logic\n\n";

    //The section should be implemented as a future works:
    //  this section is useful to describe intra row-column
    //  logic.

    Output << "\tend logic\n\n";
    //END LOGIC REGION///////////////////////////////////////


    //CELLS REGION///////////////////////////////////////////
    Output << "\tbegin cells\n\n";

    //Read the LiM Array and print the associated code
    for(limArrayIT=((*compArray).limArray).begin();limArrayIT!=((*compArray).limArray).end();++limArrayIT){

        errs() << "LimArray cycle.\n";
        identifyLIMRowAndPrint(currentRow, &limArrayIT);
        ++currentRow;
    }

    Output << "\tend cells\n\n";
    //END CELLS REGION///////////////////////////////////////


    //MAP REGION/////////////////////////////////////////////
    Output << "\tbegin map\n\n";

    //The Map section has been already defined
//    Output << OutputLiMMap.str();

    Output << "\tend map\n\n";
    //END MAP REGION/////////////////////////////////////////


    Output << "end Lim\n\n";

}

/// It prints the Map section
void PrintDexFile::printMap(){
    Output << "begin map\n\n";
    // Printing the map section: NOT Implemented.
    // This code section will be updated when the outside logic
    // will be included into the design.

    Output << "end map\n\n";

}

/// It prints the Instructions section
void PrintDexFile::printInstructions(){
    int timeCount=0;

    Output << "begin instructions\n\n";
    // Printing the instructions section

    //Declaration of all the operations implemented by the algorithm:
    for (int i=0; i<((*compFSM).getFSMSize());++i) {
        //Iteration over the Finite State Machine: each time instant has its own
        //name!
        Output << "\tLIM_INSTRUCTION Lim " << "op" << timeCount << "\n";
        timeCount++;
    }

    timeCount=0; //Not a good solution, better if the names of the operations
                 // would be gathered inside one possible structure...

    //Specification of the active blocks during the execution of a
    //declared instruction (printed in the same order):
    for (FSMIT=((*compFSM).FSM).begin(); FSMIT!=((*compFSM).FSM).end(); ++FSMIT) {
        Output << "begin " << "op" << timeCount << "\n";
        Output << "\tPIPELINE 0\n"; //No pipeline stages inside the LiM blocks

        //Power section/////////////////////////////////

        Output << "\t\tbegin power\n";

        //Declaration of the active cells, iteration over the operation list
        for (FSM_ListIT=(FSMIT->second).begin();FSM_ListIT!=(FSMIT->second).end();++FSM_ListIT) {
            Output << "\t\t\t ... \n";
        }

        Output << "\t\tend power\n\n";

        //End power section/////////////////////////////

        //Path section//////////////////////////////////
        Output << "\t\tbegin path[0]\n"; //The 0 index refers to the
                                         //first pipeline stage, the
                                         //only one implemented.

        //Declaration of the active cells
        for (;;) {
            for (;;) {

            }
            Output << "\t\t\t break\n";

        }

        Output << "\t\tend path[0]\n\n";

        //End path section//////////////////////////////

        Output << "end " << "op" << timeCount << "\n\n";
    }

    timeCount++;

    Output << "end instructions\n\n";

}

/// It prints the Code section
void PrintDexFile::printCode(){
    Output << "begin code\n\n";
    // Printing the code section
    for (;;) {

    }

    Output << "end code";
}

/// It generates the unique name for the each component: FUTURE IMPLEMENTATION
//int PrintDexFile::getIndex(std::string operation){

//    componentCntIT=componentCnt.find(operation);

//    if(componentCntIT!=componentCnt.end())
//    {
//        (componentCntIT->second)+=1;
//        return (componentCntIT->second);
//    } else {
//        componentCnt.insert({operation,0});
//        return 0;
//    }
//}

/// It modifies the passed parameters (name and index) to return
/// the requested information
void PrintDexFile::getNameAndIndexOfSourceRow(int* const &sourceRow, std::string &sourceCellName, int &sourceCellRow){

    namesMapIT=namesMap.find(sourceRow);

    if(namesMapIT!=namesMap.end())
    {
        std::string sourceName=(namesMapIT->second).name;

        //Check if the source cell is MIXED Type
        auto position = sourceName.find("Mux");
        if(position != std::string::npos){

            sourceName = "Mux";
        }

        sourceCellName=sourceName;
        sourceCellRow=(namesMapIT->second).rowIndex;
    } else {
        llvm_unreachable("Error in PrintDexFile: Reference to namesMap NOT VALID.");
    }
}

/// It inserts a new element inside the namesMap
void PrintDexFile::insertNamesMap(int* const &rowName, std::string cellName, int &cellRow){
    errs() << "Data passed: " << rowName << ", cellName " <<cellRow << "\n";
    nameAndIndex structTmp={cellName, cellRow};
    namesMap.insert({rowName, structTmp});
}


/// It identifies the kind of LiM row
void PrintDexFile::identifyLIMRowAndPrint(int &currentRow, std::map<int * const, LiMArray::LiMRow>::iterator *mapIT){

    //If the number of input connection to the row is greater than 1, a mux has to be inserted
    //inside the array
    bool isMux = ((((*mapIT)->second).inputConnections).size()>1) ? true : false;

    std::string opType;

    int * const rowName=((*mapIT)->first);

    std::map<std::string, std::string>::const_iterator OpImplIT=LimOperations.find(((*mapIT)->second).rowType);

    errs()<< "The data from the Map are: " << ((*mapIT)->second).rowType <<"\n";
    if(OpImplIT!=LimOperations.end())
    {
        errs()<<"Here we are!\n";
        opType=OpImplIT->second;


        if(opType=="null")
        {
            errs() << "A load operation is considered!\n";

            //Check if there are input operands
            if((((*mapIT)->second).inputConnections).empty())
            {
                //Source memory row
                addDataRow(currentRow, rowName, nullptr);

            } else {
                //Result memory row
                addDataRow(currentRow, rowName, (((*mapIT)->second).inputConnections).front());

            }

        } else if(opType=="bitwise"){

            if(isMux)
            {
                printBITWISEMux2to1(((*mapIT)->second).rowType, currentRow, rowName,
                                    (((*mapIT)->second).inputConnections).front(), (((*mapIT)->second).inputConnections).back());

            } else if (((*mapIT)->second).rowType=="switch"){

                printMIXED(((*mapIT)->second).rowType, currentRow, rowName,(((*mapIT)->second).inputConnections).front(),
                           ((*mapIT)->second).additionalLogic);

            } else {

                printBITWISE(((*mapIT)->second).rowType, currentRow, rowName, (((*mapIT)->second).inputConnections).front());

            }

        } else if(opType=="arith"){

            if(isMux)
            {
                printADDMux2to1(currentRow, rowName, (((*mapIT)->second).inputConnections).front(),
                         (((*mapIT)->second).inputConnections).back());

            } else {

                printADD(currentRow, rowName, (((*mapIT)->second).inputConnections).front());
            }

        } else {
            llvm_unreachable("Error in PrintDexFile: unknow operation returned from LimOperations.");
        }
    } else {
        llvm_unreachable("Error in PrintDexFile: unknown compiled operation. The name is not present"
                         "inside LimOperations map.");
    }
}

/// It is invoked when a normal memory row is declared: CHECK THE TYPE!
void PrintDexFile::addDataRow(int &currentRow, int* const &nameRow, int* const &nameSrc){

    //Name of the Output pin of the source row (if any)
    std::string outSrc;

    std::string sourceCells;
    int sourceCellsBaseIndex;

    //Update the NamesMap
    insertNamesMap(nameRow, "Memory", currentRow);

    if(nameSrc!=nullptr){
        //Result row
        getNameAndIndexOfSourceRow(nameSrc, sourceCells, sourceCellsBaseIndex);

        //Check if the source cells are FAs/HAs:
        getOutPinName(sourceCells, outSrc);

        //Also here the parallelism should refer to
        //a configuration file
        for(int i=0; i<32; ++i){

            //Map Section
            OutputLiMMap << "\t\t" << sourceCells << "("
                         << sourceCellsBaseIndex << ","
                         << i << ")." << outSrc
                         << " -> Memory(" << currentRow
                         << "," << i << ").WR\n";
        }

    } else {
        //Source row
        //Nothing is done!
    }



}

/// It prints the correct code for any BITWISE lim cell
void PrintDexFile::printBITWISE(std::string &bitwiseOp, int &currentRow, int* const &nameRow, int* const &nameSrc){

    // Support variable to store the Upper case string of bitwiseOp
    std::string implementedCell=bitwiseOp;

    //Name of the Output pin of the source row
    std::string outSrc;

    std::string sourceCells;
    int sourceCellsBaseIndex;

    getNameAndIndexOfSourceRow(nameSrc, sourceCells, sourceCellsBaseIndex);

    //Check if the source cells are FAs/HAs:
    getOutPinName(sourceCells, outSrc);

    //Impose Upper case conditon to implementedCell
    transform(implementedCell.begin(), implementedCell.end(), implementedCell.begin(), ::toupper);

    //Also here the parallelism should refer to
    //a configuration file
    for(int i=0; i<32; ++i){

        //If the first MSB is considered
        if(i==0){
            insertNamesMap(nameRow, bitwiseOp, currentRow);
        }

        //Cells Section
        Output << "\t\t" << implementedCell << " "
               << bitwiseOp << "(2) -> Cell(" << currentRow
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << currentRow
                     << "," << i << ").RD -> " << bitwiseOp << "("
                     << currentRow << "," << i << ").IN0\n";
        OutputLiMMap << "\t\t" << sourceCells << "("
                     << sourceCellsBaseIndex << ","
                     << i << ")." << outSrc << " -> " << bitwiseOp
                     << "(" << currentRow << "," << i
                     << ").IN1\n";
    }

}

/// It prints the correct code for an SUM lim cell
void PrintDexFile::printADD(int &currentRow, int* const &nameRow, int* const &nameSrc){

    std::string sourceCells;
    int sourceCellsBaseIndex;

    //Name of the Output pin of the source row
    std::string outSrc;

    //Index for the for loop
    int i;

    getNameAndIndexOfSourceRow(nameSrc, sourceCells, sourceCellsBaseIndex);

    //Check if the source cells are FAs/HAs:
    getOutPinName(sourceCells, outSrc);

    //Also here the parallelism should refer to
    //a configuration file
    for(i=0; i<31; ++i){

        //If the first MSB is considered
        if(i==0){
            insertNamesMap(nameRow, "Add", currentRow);
        }

        //Cells Section
        Output << "\t\tFA Fa -> Cell(" << currentRow
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << currentRow
                     << "," << i << ").RD -> Fa("
                     << currentRow << "," << i << ").A\n";
        OutputLiMMap << "\t\t" << sourceCells << "("
                     << sourceCellsBaseIndex << ","
                     << i << ")." << outSrc << " -> Fa("
                     << currentRow << "," << i
                     << ").B\n";
        OutputLiMMap << "\t\tFa(" << currentRow
                     << "," << (i-1) << ").COUT -> Fa("
                     << currentRow << "," << i
                     << ").CIN\n";
    }

    //The last element (LSB) is an Half adder
    ++i;

    //Cells Section
    Output << "\t\tHA Ha -> Cell(" << currentRow
           << "," << i << ")\n";

    //Map Section
    OutputLiMMap << "\t\tMemory(" << currentRow
                 << "," << i << ").RD -> Ha("
                 << currentRow << "," << i << ").A\n";
    OutputLiMMap << "\t\t" << sourceCells << "("
                 << sourceCellsBaseIndex << ","
                 << i << ")." << outSrc << " -> Ha("
                 << currentRow << "," << i
                 << ").B\n";

}

/// It prints the correct code for any MIXED lim cell
void PrintDexFile::printMIXED(std::string &bitwiseOp, int &currentRow, int* const &nameRow,
                              int* const &nameSrc, std::list<std::string> &operators){

    // Support variable to store the Upper case string of bitwiseOp
    std::string implementedCell;

    // Support variable to store the row name
    std::string rowName="Mux_";

    //Out mux dimension
    int muxDimension=operators.size();

    //Counter for mux inputs
    int muxCount=0;

    if((muxDimension & (muxDimension-1))!=0)
    {
        //Error in the definition of the input mux: not a power of two!
        llvm_unreachable("Error in PrintDexFile: the mux of the LiM row is not properly defined!");
    }

    //Name of the Output pin of the source row
    std::string outSrc;

    std::string sourceCells;
    int sourceCellsBaseIndex;

    getNameAndIndexOfSourceRow(nameSrc, sourceCells, sourceCellsBaseIndex);

    //Check if the source cells are FAs/HAs:
    getOutPinName(sourceCells, outSrc);

    //Also here the parallelism should refer to
    //a configuration file
    for(int i=0; i<31; ++i){

        //If the first MSB is considered
        if(i==0){

            //Definition of the name of the row (concat of the operators at the input of the mux)
            for(std::list<std::string>::iterator tmpIT=operators.begin(); tmpIT!=operators.end(); ++tmpIT)
            {
                rowName=rowName+(*tmpIT);
                if(tmpIT!=std::next(operators.end(), -1))
                {
                    rowName=rowName+"_";
                }
            }
            insertNamesMap(nameRow, rowName, currentRow);
        }

        //Cells Section
        for(std::list<std::string>::iterator tmpIT=operators.begin(); tmpIT!=operators.end(); ++tmpIT)
        {
            implementedCell=*tmpIT;

            //Impose Upper case conditon to implementedCell
            transform(implementedCell.begin(), implementedCell.end(), implementedCell.begin(), ::toupper);

            Output << "\t\t" << implementedCell << " "
                   << *tmpIT << "(2) -> Cell(" << currentRow
                   << "," << i << ")\n";

            //Map Section up to mux input
            OutputLiMMap << "\t\tMemory(" << currentRow
                         << "," << i << ").RD -> " << *tmpIT << "("
                         << currentRow << "," << i << ").IN0\n";
            OutputLiMMap << "\t\t" << sourceCells << "("
                         << sourceCellsBaseIndex << ","
                         << i << ")." << outSrc << " -> " << *tmpIT
                         << "(" << currentRow << "," << i << ").IN1\n";
            OutputLiMMap <<"\t\t" << *tmpIT << "(" << currentRow << ","
                         << i << ").OUT0 -> Mux(" << currentRow
                         << "," << i << ").IN" << muxCount << "\n";
        }

        //Definition of the output multiplexer
        Output << "\t\tMUX Mux(" << muxDimension << ",1," << log2(muxDimension)
               << ") -> Cell(" << currentRow << "," << i << ")\n";

    }

}

/// It prints the correct code for any BITWISE lim cell with a 2to1 mux in input
/// so that 2 input bits for the logic are available.
///     (NOTE:Check the definition of the multiplexer!)
void PrintDexFile::printBITWISEMux2to1(std::string &bitwiseOp, int &currentRow, int* const &nameRow, int* const &nameSrc1, int* const &nameSrc2){

    // Support variable to store the Upper case string of bitwiseOp
    std::string implementedCell=bitwiseOp;

    //Name of the Output pin of the source rows
    std::string outSrc1;
    std::string outSrc2;

    std::string sourceCells1;
    int sourceCellsBaseIndex1;

    std::string sourceCells2;
    int sourceCellsBaseIndex2;

    getNameAndIndexOfSourceRow(nameSrc1, sourceCells1, sourceCellsBaseIndex1);
    getNameAndIndexOfSourceRow(nameSrc2, sourceCells2, sourceCellsBaseIndex2);

    //Check if the source cells are FAs/HAs:
    getOutPinName(sourceCells1, outSrc1);
    getOutPinName(sourceCells2, outSrc2);

    //Impose Upper case conditon to implementedCell
    transform(implementedCell.begin(), implementedCell.end(), implementedCell.begin(), ::toupper);

    //Also here the parallelism should refer to
    //a configuration file
    for(int i=0; i<32; ++i){

        //If the first MSB is considered
        if(i==0){
            insertNamesMap(nameRow, bitwiseOp+"Mux", currentRow);
        }

        //Cells Section
        Output << "\t\t" << implementedCell << " "
               << bitwiseOp << "(2) -> Cell(" << currentRow
               << "," << i << ")\n";
        Output << "\t\tMUX Mux(2,1,1) -> Cell(" << currentRow
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << currentRow
                     << "," << i << ").RD -> " << bitwiseOp <<"("
                     << currentRow << "," << i << ").IN0\n";
        OutputLiMMap << "\t\tMux(" << currentRow
                     << "," << i << ").OUT0 -> " << bitwiseOp <<"("
                     << currentRow << "," << i << ").IN1\n";
        OutputLiMMap << "\t\t" << sourceCells1 << "("
                     << sourceCellsBaseIndex1 << ","
                     << i << ")." << outSrc1 << " -> Mux("
                     << currentRow << "," << i
                     << ").IN0\n";
        OutputLiMMap << "\t\t" << sourceCells2 << "("
                     << sourceCellsBaseIndex2 << ","
                     << i << ")." << outSrc2 << " -> Mux("
                     << currentRow << "," << i
                     << ").IN1\n";
    }
}

/// It prints the correct code for an SUM lim cell with a 2to1 mux in input
/// so that 2 input bits for the logic are available.
void PrintDexFile::printADDMux2to1(int &currentRow, int* const &nameRow, int* const &nameSrc1, int* const &nameSrc2){

    std::string sourceCells1;
    int sourceCellsBaseIndex1;

    std::string sourceCells2;
    int sourceCellsBaseIndex2;

    //Name of the Output pin of the source rows
    std::string outSrc1;
    std::string outSrc2;

    //Index for the for loop
    int i;

    getNameAndIndexOfSourceRow(nameSrc1, sourceCells1, sourceCellsBaseIndex1);
    getNameAndIndexOfSourceRow(nameSrc2, sourceCells2, sourceCellsBaseIndex2);

    //Check if the source cells are FAs/HAs:
    getOutPinName(sourceCells1, outSrc1);
    getOutPinName(sourceCells2, outSrc2);

    //Also here the parallelism should refer to
    //a configuration file
    for(i=0; i<31; ++i){

        //If the first MSB is considered
        if(i==0){
            insertNamesMap(nameRow, "AddMux", currentRow);
        }

        //Cells Section
        Output << "\t\tFA Fa -> Cell(" << currentRow
               << "," << i << ")\n";
        Output << "\t\tMUX Mux(2,1,1) -> Cell(" << currentRow
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << currentRow
                     << "," << i << ").RD -> Fa("
                     << currentRow << "," << i << ").A\n";
        OutputLiMMap << "\t\tMux(" << currentRow
                     << "," << i << ").OUT0 -> Fa("
                     << currentRow << "," << i << ").B\n";
        OutputLiMMap << "\t\t" << sourceCells1 << "("
                     << sourceCellsBaseIndex1 << ","
                     << i << ")." << outSrc1 << " -> Mux("
                     << currentRow << "," << i
                     << ").IN0\n";
        OutputLiMMap << "\t\t" << sourceCells2 << "("
                     << sourceCellsBaseIndex2 << ","
                     << i << ")." << outSrc2 << " -> Mux("
                     << currentRow << "," << i
                     << ").IN1\n";
        OutputLiMMap << "\t\tFa(" << currentRow
                     << "," << (i-1) << ").COUT -> Fa("
                     << currentRow << "," << i
                     << ").CIN\n";
    }

    //The last element (LSB) is an Half adder
    ++i;
    //Cells Section
    Output << "\t\tHA Ha -> Cell(" << currentRow
           << "," << i << ")\n";

    //Map Section
    //Supposing (IN0=a, IN1=b, CIN=carry, OUT=SUM, COUT= carry out): CHECK!
    OutputLiMMap << "\t\tMemory(" << currentRow
                 << "," << i << ").RD -> Ha("
                 << currentRow << "," << i << ").A\n";
    OutputLiMMap << "\t\tMux(" << currentRow
                 << "," << i << ").OUT0 -> Ha("
                 << currentRow << "," << i << ").B\n";
    OutputLiMMap << "\t\t" << sourceCells1 << "("
                 << sourceCellsBaseIndex1 << ","
                 << i << ")." << outSrc1 << " -> Mux("
                 << currentRow << "," << i
                 << ").IN0\n";
    OutputLiMMap << "\t\t" << sourceCells2 << "("
                 << sourceCellsBaseIndex2 << ","
                 << i << ")." << outSrc2 << " -> Mux("
                 << currentRow << "," << i
                 << ").IN1\n";
}

/// It prints the correct code for any MIXED lim cell with a 2to1 mux in input
void PrintDexFile::printMIXEDMux2to1(int &currentRow, int * const &nameRow, int* const &nameSrc1, int* const &nameSrc2,
                                     std::list<std::string> &operators){

}

/// It returns the correct out pin name of opSrc row:
/// it's useful for the identification of the output pin name
/// of the row in case of is an Arithmetic Operation.
void PrintDexFile::getOutPinName(std::string &opSrc, std::string &outPinName){

    std::string opSrcInt=opSrc;
    std::map<std::string, std::string>::const_iterator tmpIT;

    auto position = opSrc.find("Mux");
    if(position != std::string::npos)
    {
        opSrcInt = opSrc.substr(0,position);
    }

    //Impose Lower case conditon to implementedCell
    transform(opSrcInt.begin(), opSrcInt.end(), opSrcInt.begin(), ::tolower);

    tmpIT=LimOperations.find(opSrcInt);

    if(tmpIT!=LimOperations.end()&&((tmpIT->second)=="arith")){
        outPinName="S";
    } else {
        outPinName="OUT";
    }
}

/// It prints the correct code for an SUB lim cell: CRITICAL, there's the need
/// of an additional bit as CIN. The sub is converted into an addition by the
/// preceding passes.
//void PrintDexFile::printSUB(int * currentRow, int **nameRow, int **nameSrc){

//    std::string sourceCells;
//    int sourceCellsBaseIndex;

//    getNameAndIndexOfSourceRow(nameSrc, &sourceCells, &sourceCellsBaseIndex);

//    //Also here the parallelism should refer to
//    //a configuration file
//    for(int i=0; i<31; ++i){

//        //If the first MSB is considered
//        if(i==0){
//            insertNamesMap(nameRow, "Sub", currentRow);
//        }

//        //Cells Section
//        Output << "\t\tFA Fa -> Cell(" << (*currentRow)
//               << "," << i << ")\n";

//        //Map Section
//        //Supposing (IN0=a, IN1=b, CIN=carry, OUT=SUM, COUT= carry out): CHECK!
//        OutputLiMMap << "\t\tMemory(" << (*currentRow)
//                     << "," << i << ").RD -> Fa("
//                     << (*currentRow) << "," << i << ").IN0\n";
//        OutputLiMMap << "\t\t" << sourceCells << "("
//                     << sourceCellsBaseIndex << ","
//                     << i << ").OUT -> Fa("
//                     << (*currentRow) << "," << i
//                     << ").IN1\n";
//        OutputLiMMap << "\t\tFa(" << (*currentRow)
//                     << "," << (i-1) << ").COUT -> Fa("
//                     << (*currentRow) << "," << i
//                     << ").CIN\n";
//    }

//}
