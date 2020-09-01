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

using namespace octantis;
using namespace llvm;

/// It prints the .dex file
void PrintDexFile::print(){

    printConstants();
    printInit();
    printLim();

    //Add the remaining sections!


}

/// It prints the Constants section
void PrintDexFile::printConstants(){
    Output << "begin constants\n";
    // Printing the CONSTANTS section of Dexima's conf.file

    //The section should be implemented as a future works

    Output << "end constants\n\n";
}

/// It prints the Init section
void PrintDexFile::printInit(){
    Output << "begin init\n";
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


    Output << "begin Lim\n";
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

        identifyLIMRowAndPrint(&currentRow, &limArrayIT);
    }

    Output << "\tend cells\n\n";
    //END CELLS REGION///////////////////////////////////////


    //MAP REGION/////////////////////////////////////////////
    Output << "\tbegin map\n\n";

    //The Map section has been already defined
    Output << OutputLiMMap.str();

    Output << "\tend map\n\n";
    //END MAP REGION/////////////////////////////////////////


    Output << "end Lim\n\n";

}

/// It prints the Map section
void PrintDexFile::printMap(){
    Output << "begin map";
    // Printing the map section

    Output << "end map";

}

/// It prints the Instructions section
void PrintDexFile::printInstructions(){
    Output << "begin instructions";
    // Printing the instructions section

    Output << "end instructions";

}

/// It prints the Code section
void PrintDexFile::printCode(){
    Output << "begin code";
    // Printing the code section

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
void PrintDexFile::getNameAndIndexOfSourceRow(int ** sourceRow, std::string * sourceCellName, int * sourceCellRow){

    namesMapIT=namesMap.find(*sourceRow);

    if(namesMapIT!=namesMap.end())
    {
        *sourceCellName=(namesMapIT->second).name;
        *sourceCellRow=(namesMapIT->second).rowIndex;
    } else {
        llvm_unreachable("Error in PrintDexFile: Reference to namesMap NOT VALID.");
    }
}

/// It inserts a new element inside the the namesMap
void PrintDexFile::insertNamesMap(int ** rowName, std::string cellName, int * cellRow){
    nameAndIndex structTmp={cellName,*cellRow};
    namesMap.insert({*rowName,structTmp});
}


/// It identifies the kind of LiM row
void PrintDexFile::identifyLIMRowAndPrint(int * currentRow, std::map<int *, LiMArray::LiMRow>::iterator *mapIT){

    //If the number of input connection to the row is greater than 1, a mux has to be inserted
    //inside the array
    bool isMux = ((((*mapIT)->second).inputConnections).size()>1) ? true : false;

    std::string opType;

    int * rowName=((*mapIT)->first);

    std::map<std::string, std::string>::const_iterator OpImplIT=LimOperations.find(((*mapIT)->second).rowType);

    if(OpImplIT!=LimOperations.end())
    {
        opType=OpImplIT->second;

        if(opType=="null")
        {
            addDataRow(currentRow, &rowName);

        } else if(opType=="bitwise"){

            if(isMux)
            {
                printBITWISEMux2to1(((*mapIT)->second).rowType, currentRow, &rowName,
                                    &((((*mapIT)->second).inputConnections).front()), &((((*mapIT)->second).inputConnections).back()));

            } else {

                printBITWISE(((*mapIT)->second).rowType, currentRow, &rowName, &((((*mapIT)->second).inputConnections).front()));

            }

        } else if(opType=="arith"){

            if(isMux)
            {
                printADDMux2to1(currentRow, &rowName, &((((*mapIT)->second).inputConnections).front()),
                         &((((*mapIT)->second).inputConnections).back()));

            } else {

                printADD(currentRow, &rowName, &((((*mapIT)->second).inputConnections).front()));
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
void PrintDexFile::addDataRow(int * currentRow, int ** nameRow){
    insertNamesMap(nameRow, "Load", currentRow);
}

/// It prints the correct code for any BITWISE lim cell
void PrintDexFile::printBITWISE(std::string bitwiseOp, int * currentRow, int ** nameRow, int ** nameSrc){

    // Support variable to store the Upper case string of bitwiseOp
    std::string implementedCell=bitwiseOp;

    //Name of the Output pin of the source row
    std::string outSrc;

    std::string sourceCells;
    int sourceCellsBaseIndex;

    getNameAndIndexOfSourceRow(nameSrc, &sourceCells, &sourceCellsBaseIndex);

    //Check if the source cells are FAs/HAs:
    getOutPinName(&sourceCells, &outSrc);

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
               << bitwiseOp << "(2) -> Cell(" << (*currentRow)
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << (*currentRow)
                     << "," << i << ").RD -> " << bitwiseOp << "("
                     << (*currentRow) << "," << i << ").IN0\n";
        OutputLiMMap << "\t\t" << sourceCells << "("
                     << sourceCellsBaseIndex << ","
                     << i << ")." << outSrc << " -> " << bitwiseOp
                     << "(" << (*currentRow) << "," << i
                     << ").IN1\n";
    }

}

/// It prints the correct code for an SUM lim cell
void PrintDexFile::printADD(int * currentRow, int **nameRow, int **nameSrc){

    std::string sourceCells;
    int sourceCellsBaseIndex;

    //Name of the Output pin of the source row
    std::string outSrc;

    //Index for the for loop
    int i;

    getNameAndIndexOfSourceRow(nameSrc, &sourceCells, &sourceCellsBaseIndex);

    //Check if the source cells are FAs/HAs:
    getOutPinName(&sourceCells, &outSrc);

    //Also here the parallelism should refer to
    //a configuration file
    for(i=0; i<31; ++i){

        //If the first MSB is considered
        if(i==0){
            insertNamesMap(nameRow, "Add", currentRow);
        }

        //Cells Section
        Output << "\t\tFA Fa -> Cell(" << (*currentRow)
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << (*currentRow)
                     << "," << i << ").RD -> Fa("
                     << (*currentRow) << "," << i << ").A\n";
        OutputLiMMap << "\t\t" << sourceCells << "("
                     << sourceCellsBaseIndex << ","
                     << i << ")." << outSrc << " -> Fa("
                     << (*currentRow) << "," << i
                     << ").B\n";
        OutputLiMMap << "\t\tFa(" << (*currentRow)
                     << "," << (i-1) << ").COUT -> Fa("
                     << (*currentRow) << "," << i
                     << ").CIN\n";
    }

    //The last element (LSB) is an Half adder
    ++i;

    //Cells Section
    Output << "\t\tHA Ha -> Cell(" << (*currentRow)
           << "," << i << ")\n";

    //Map Section
    OutputLiMMap << "\t\tMemory(" << (*currentRow)
                 << "," << i << ").RD -> Ha("
                 << (*currentRow) << "," << i << ").A\n";
    OutputLiMMap << "\t\t" << sourceCells << "("
                 << sourceCellsBaseIndex << ","
                 << i << ")." << outSrc << " -> Ha("
                 << (*currentRow) << "," << i
                 << ").B\n";

}

/// It prints the correct code for any BITWISE lim cell with a 2to1 mux in input
/// so that 2 input bits for the logic are available.
void PrintDexFile::printBITWISEMux2to1(std::string bitwiseOp, int * currentRow, int **nameRow, int **nameSrc1, int **nameSrc2){

    // Support variable to store the Upper case string of bitwiseOp
    std::string implementedCell=bitwiseOp;

    //Name of the Output pin of the source rows
    std::string outSrc1;
    std::string outSrc2;

    std::string sourceCells1;
    int sourceCellsBaseIndex1;

    std::string sourceCells2;
    int sourceCellsBaseIndex2;

    getNameAndIndexOfSourceRow(nameSrc1, &sourceCells1, &sourceCellsBaseIndex1);
    getNameAndIndexOfSourceRow(nameSrc2, &sourceCells2, &sourceCellsBaseIndex2);

    //Check if the source cells are FAs/HAs:
    getOutPinName(&sourceCells1, &outSrc1);
    getOutPinName(&sourceCells2, &outSrc2);

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
               << bitwiseOp << "(2) -> Cell(" << (*currentRow)
               << "," << i << ")\n";
        Output << "\t\tMUX Mux(2,1,2) -> Cell(" << (*currentRow)
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << (*currentRow)
                     << "," << i << ").RD -> " << bitwiseOp <<"("
                     << (*currentRow) << "," << i << ").IN0\n";
        OutputLiMMap << "\t\tMux(" << (*currentRow)
                     << "," << i << ").OUT0 -> " << bitwiseOp <<"("
                     << (*currentRow) << "," << i << ").IN1\n";
        OutputLiMMap << "\t\t" << sourceCells1 << "("
                     << sourceCellsBaseIndex1 << ","
                     << i << ")." << outSrc1 << " -> Mux("
                     << (*currentRow) << "," << i
                     << ").IN0\n";
        OutputLiMMap << "\t\t" << sourceCells2 << "("
                     << sourceCellsBaseIndex2 << ","
                     << i << ")." << outSrc2 << " -> Mux("
                     << (*currentRow) << "," << i
                     << ").IN1\n";
    }
}

/// It prints the correct code for an SUM lim cell with a 2to1 mux in input
/// so that 2 input bits for the logic are available.
void PrintDexFile::printADDMux2to1(int * currentRow, int **nameRow, int **nameSrc1, int **nameSrc2){

    std::string sourceCells1;
    int sourceCellsBaseIndex1;

    std::string sourceCells2;
    int sourceCellsBaseIndex2;

    //Name of the Output pin of the source rows
    std::string outSrc1;
    std::string outSrc2;

    //Index for the for loop
    int i;

    getNameAndIndexOfSourceRow(nameSrc1, &sourceCells1, &sourceCellsBaseIndex1);
    getNameAndIndexOfSourceRow(nameSrc2, &sourceCells2, &sourceCellsBaseIndex2);

    //Check if the source cells are FAs/HAs:
    getOutPinName(&sourceCells1, &outSrc1);
    getOutPinName(&sourceCells2, &outSrc2);

    //Also here the parallelism should refer to
    //a configuration file
    for(i=0; i<31; ++i){

        //If the first MSB is considered
        if(i==0){
            insertNamesMap(nameRow, "AddMux", currentRow);
        }

        //Cells Section
        Output << "\t\tFA Fa -> Cell(" << (*currentRow)
               << "," << i << ")\n";
        Output << "\t\tMUX Mux(2,1,2) -> Cell(" << (*currentRow)
               << "," << i << ")\n";

        //Map Section
        OutputLiMMap << "\t\tMemory(" << (*currentRow)
                     << "," << i << ").RD -> Fa("
                     << (*currentRow) << "," << i << ").A\n";
        OutputLiMMap << "\t\tMux(" << (*currentRow)
                     << "," << i << ").OUT0 -> Fa("
                     << (*currentRow) << "," << i << ").B\n";
        OutputLiMMap << "\t\t" << sourceCells1 << "("
                     << sourceCellsBaseIndex1 << ","
                     << i << ")." << outSrc1 << " -> Mux("
                     << (*currentRow) << "," << i
                     << ").IN0\n";
        OutputLiMMap << "\t\t" << sourceCells2 << "("
                     << sourceCellsBaseIndex2 << ","
                     << i << ")." << outSrc2 << " -> Mux("
                     << (*currentRow) << "," << i
                     << ").IN1\n";
        OutputLiMMap << "\t\tFa(" << (*currentRow)
                     << "," << (i-1) << ").COUT -> Fa("
                     << (*currentRow) << "," << i
                     << ").CIN\n";
    }

    //The last element (LSB) is an Half adder
    ++i;
    //Cells Section
    Output << "\t\tHA Ha -> Cell(" << (*currentRow)
           << "," << i << ")\n";

    //Map Section
    //Supposing (IN0=a, IN1=b, CIN=carry, OUT=SUM, COUT= carry out): CHECK!
    OutputLiMMap << "\t\tMemory(" << (*currentRow)
                 << "," << i << ").RD -> Ha("
                 << (*currentRow) << "," << i << ").A\n";
    OutputLiMMap << "\t\tMux(" << (*currentRow)
                 << "," << i << ").OUT0 -> Ha("
                 << (*currentRow) << "," << i << ").B\n";
    OutputLiMMap << "\t\t" << sourceCells1 << "("
                 << sourceCellsBaseIndex1 << ","
                 << i << ")." << outSrc1 << " -> Mux("
                 << (*currentRow) << "," << i
                 << ").IN0\n";
    OutputLiMMap << "\t\t" << sourceCells2 << "("
                 << sourceCellsBaseIndex2 << ","
                 << i << ")." << outSrc2 << " -> Mux("
                 << (*currentRow) << "," << i
                 << ").IN1\n";
}

/// It returns the correct out pin name of opSrc row:
/// it's useful for the identification of the output pin name
/// of the row in case of is an Arithmetic Operation.
void PrintDexFile::getOutPinName(std::string * opSrc, std::string * outPinName){

    std::string opSrcInt=(*opSrc);
    std::map<std::string, std::string>::const_iterator tmpIT;

    auto position = (*opSrc).find("Mux");
    if(position != std::string::npos)
    {
        opSrcInt = (*opSrc).substr(0,position);
    }

    //Impose Lower case conditon to implementedCell
    transform(opSrcInt.begin(), opSrcInt.end(), opSrcInt.begin(), ::tolower);

    tmpIT=LimOperations.find(opSrcInt);

    if(tmpIT!=LimOperations.end()&&((tmpIT->second)=="arith")){
        (*outPinName)="S";
    } else {
        (*outPinName)="OUT";
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
