// Standard C++ Include Files
#include <algorithm>
#include <fstream>
#include <iostream>
#include <system_error>

// Debug
#include <stdexcept>

// LLVM include files
#include "PrintCadFile.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"

using namespace octantis;
using namespace llvm;

void PrintCadFile::print(){

    printArray();

    printLimcadCsvFile();
    printIrlcadCsvFile();
}

/// It prints the the first .limcad (describing cell architecture) file for a Lim Cell containing a logic operator
void PrintCadFile::printBitwiseLimcad(std::string &logicOp){
    // .limcad file opening with error handling
    std::string limcadCellName = "LIM_Bitwise_cell.limcad";
    std::error_code ErrorInfolimcad;
    limcadCellFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the Bitwise cell .limcad file.");
    }

    // transform the operator string from lower case to upper case
    transform(logicOp.begin(), logicOp.end(), logicOp.begin(), ::toupper);
    // label for the operator
    std::string logicOpLabel = logicOp + "2_10";

    // PINS section

    *limcadCellFile <<  "BL 0 0 Ext 5 Input\n"
                        " BL[1] : \n"
                        "CLK 0 0 Ext 5 Input\n"
                        " CLK[1] : \n"
                        "RST 0 0 Ext 5 Input\n"
                        " RST[1] : \n"
                        "WL 0 0 Ext 5 Input\n"
                        " WL[1] : \n"
                        "OC 0 0 Ext 5 Output\n"
                        " OC[1] : \n"
                        "OLiM 0 0 Ext 5 Output\n"
                        " OLiM[1] : \n\n";

    // COMPONENT section

    // cell declaration
    *limcadCellFile <<  "Cell_9 0 0 Cell 1\n"
                        " CK[1] : CLK.CLK \n"
                        " EN[1] : WL.WL \n"
                        " R[1] : RST.RST \n"
                        " D[1] : BL.BL \n"
                        " Q[1] :" << logicOpLabel << ".IN0\n\n";

    // logic operator declaration
    *limcadCellFile <<  logicOpLabel << "0 0 " << logicOp << " 0\n"
                        " IN0[1] : \n"
                        " IN1[1] : BL.BL \n"
                        " O[1] : OLiM.OLiM\n\n";

    // Closing the file
    (*limcadCellFile).close();                       
}

/// It prints the the first .limcad (describing cell architecture) file for a Lim Cell containing a logic operator
/// and an input 2-to-1 mux
void PrintCadFile::printBitwiseMux2to1Limcad(std::string &logicOp){
    // .limcad file opening with error handling
    std::string limcadCellName = "LIM_BitwiseMux2to1_cell.limcad";
    std::error_code ErrorInfolimcad;
    limcadCellFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the Mux2to1 Bitwise cell .limcad file.");
    }

    // transform the operator string from lower case to upper case
    transform(logicOp.begin(), logicOp.end(), logicOp.begin(), ::toupper);
    // label for the operator
    std::string logicOpLabel = logicOp + "2_10";

    // PINS section

    *limcadCellFile <<  "BL1 0 0 Ext 5 Input\n"
                        " BL1[1] : \n"
                        "BL2 0 0 Ext 5 Input\n"
                        " BL2[1] : \n"
                        "CLK 0 0 Ext 5 Input\n"
                        " CLK[1] : \n"
                        "S0 760.0 920.0 Ext 5 Input\n"
                        " S0[1] : \n"
                        "RST 0 0 Ext 5 Input\n"
                        " RST[1] : \n"
                        "WL 0 0 Ext 5 Input\n"
                        " WL[1] : \n"
                        "OC 0 0 Ext 5 Output\n"
                        " OC[1] : \n"
                        "OLiM 0 0 Ext 5 Output\n"
                        " OLiM[1] : \n\n";

    // COMPONENT section

    // cell declaration
    *limcadCellFile <<  "Cell_9 0 0 Cell 1\n"
                        " CK[1] : CLK.CLK \n"
                        " EN[1] : WL.WL \n"
                        " R[1] : RST.RST \n"
                        " D[1] : BL.BL \n"
                        " Q[1] :" << logicOpLabel << ".IN0\n\n";

    // 2to1 mux declaration
    *limcadCellFile <<  "MUX21_11 0 0 MUX21 4\n"
                        " IN0[1] : BL1.BL1\n"
                        " IN1[1] : BL2.BL2\n"
                        " S[1] : S0.S0 \n"
                        " O[1] :" << logicOpLabel << ".IN1\n\n";

    // logic operator declaration
    *limcadCellFile <<  logicOpLabel << "0 0 " << logicOp << " 0\n"
                        " IN0[1] : \n"
                        " IN1[1] : \n"
                        " O[1] : OLiM.OLiM\n\n";

    // Closing the file
    (*limcadCellFile).close();   
}

/// It prints the the first .limcad (describing cell architecture) file for a Lim Cell containing multitple logic operators
void PrintCadFile::printMixedLimcad(std::list<std::string> &operators){
    // .limcad file opening with error handling
    std::string limcadCellName = "LIM_BitwiseMixed_cell.limcad";
    std::error_code ErrorInfolimcad;
    limcadCellFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the Mixed Bitwise cell .limcad file.");
    }

    // list containing all the operators labels
    std::list<std::string> opLabelList;

    int count = 0;

    // Out mux dimension
    int muxDimension = operators.size();

    // transform each operator string from lower case to upper case
    for(auto listIT = operators.begin(); listIT != operators.end(); ++listIT){
        transform((*listIT).begin(), (*listIT).end(), (*listIT).begin(), ::toupper);
        // pushback the label for the operator in opLabelList
        opLabelList.push_back( (*listIT) + "2_1" + std::to_string(count));
        count++;
    }

    // PINS section

    *limcadCellFile <<  "BL1 0 0 Ext 5 Input\n"
                        " BL1[1] : \n"
                        "CLK 0 0 Ext 5 Input\n"
                        " CLK[1] : \n"
                        "S0 0 0 Ext 5 Input\n"
                        " S0[" << muxDimension << "] : \n"
                        "RST 0 0 Ext 5 Input\n"
                        " RST[1] : \n"
                        "WL 0 0 Ext 5 Input\n"
                        " WL[1] : \n"
                        "OC 0 0 Ext 5 Output\n"
                        " OC[1] : \n"
                        "OLiM 0 0 Ext 5 Output\n"
                        " OLiM[1] : \n\n";

    // COMPONENT section

    // cell declaration
    *limcadCellFile <<  "Cell_9 0 0 Cell 1\n"
                        " CK[1] : CLK.CLK \n"
                        " EN[1] : WL.WL \n"
                        " R[1] : RST.RST \n"
                        " D[1] : BL.BL \n"
                        " Q[1] :" ;

    for(auto listIT = opLabelList.begin(); listIT != opLabelList.end(); ++listIT){
        *limcadCellFile << (*listIT) << ".IN0, ";
    }

    // 2to1 mux declaration
    *limcadCellFile <<  "MUX21_2 0 0 MUX21 4\n";

    count = 0;
    for(auto listIT = opLabelList.begin(); listIT != opLabelList.end(); ++listIT){
        *limcadCellFile << "IN" << std::to_string(count) << "[1] : " << (*listIT) << ".O\n";
        count++;
    }                 

    *limcadCellFile <<  " S[1] : S0.S0 \n"
                        " O[1] : OLiM.OLiM\n\n";

    // logic operators declaration

    count = 0;
    for(auto opListIT = operators.begin(); opListIT != operators.end(); ++opListIT){
        *limcadCellFile << (*opListIT) << "2_1" << std::to_string(count) << " 0 0 " << (*opListIT) <<" 0\n"
                        " IN0[1] : Cell_9.Q\n"
                        " IN1[1] : BL.BL\n"
                        " O[1] : MUX21_2.IN" << std::to_string(count) << "\n\n";
        count++;
    }
    

    // Closing the file
    (*limcadCellFile).close();   
}

/// It prints the the first .limcad (describing cell architecture) file for a Lim Cell containing a FA
void PrintCadFile::printAddLimcad(){
    // .limcad file opening with error handling
    std::string limcadCellName = "LIM_Memory_cell.limcad";
    std::error_code ErrorInfolimcad;
    limcadCellFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the Memory cell .limcad file associated to an ADD row.");
    }

    // printing the pins and declaring the memory cell inside the .limcad file
    printMemoryLimcad(limcadCellFile, false);
    // printing the associated .irlcad file
    printAddIrlcadFile();

    (*limcadCellFile).close();
}

/// It prints the the first .limcad (describing cell architecture) file for a Lim Cell containing a FA
/// but also an input 2-to-1 mux
void PrintCadFile::printAddMux2to1Limcad(){
    // .limcad file opening with error handling
    std::string limcadCellName = "LIM_MemoryMux2to1_cell.limcad";
    std::error_code ErrorInfolimcad;
    limcadCellFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the Memory cell .limcad file associated to an AddMux2to1 row.");
    }

    // printing the pins and declaring the memory cell inside the .limcad file
    printMemoryLimcad(limcadCellFile, true);

    *limcadCellFile <<   "MUX21_12 0 0 MUX21 4\n"
                        " IN0[1] : BL1[1]\n"
                        " IN1[1] : BL2[2]\n"
                        " S[1] : S0.S0 \n"
                        " O[1] : OLiM.OLiM "; 

    (*limcadCellFile).close();

}

/// It prints  the first .limcad (describing cell architecture) file for a Lim Cell contained in an intermediate result row
/// generated by Octantis. As a matter of fact, it consists in a simple memory cell
void PrintCadFile::printResultLimcad(){
    // .limcad file opening with error handling
    std::string limcadCellName = "LIM_Memory_cell.limcad";
    std::error_code ErrorInfolimcad;
    limcadCellFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the Memory cell .limcad file.");
    }

    printMemoryLimcad(limcadCellFile, false);

    (*limcadCellFile).close();
}

/// It prints the the first .limcad (describing cell architecture) file for a Lim Cell containing only a memory cell 
void PrintCadFile::printMemoryLimcad(raw_fd_ostream *const &fd, bool isMux){
    // PIN SECTION

    // if an input 2to1 mux is needed, BL1 and BL2 are required
    if(!isMux){
        *fd <<  "BL 0 0 Ext 5 Input\n"
                " BL[1] : \n";
    }else{
        *fd <<  "BL1 0 0 Ext 5 Input\n"
                " BL1[1] : \n"
                "BL2 0 0 Ext 5 Input\n"
                " BL2[1] : \n";
    }

    *fd <<  "CLK 0 0 Ext 5 Input\n"
            " CLK[1] : \n"
            "RST 0 0 Ext 5 Input\n"
            " RST[1] : \n"
            "WL 0 0 Ext 5 Input\n"
            " WL[1] : \n";

    // a selection signal is added for the input 2to1mux
    if(isMux){
        *fd <<  "S0 0 0 Ext 5 Input\n"
                " S0[1] : ";
    }
    
    *fd <<  "OC 0 0 Ext 5 Output\n"
            " OC[1] : \n"
            "OLiM 0 0 Ext 5 Output\n"
            " OLiM[1] : \n\n";

    // COMPONENT section

    // cell declaration
    *fd <<  "Cell_9 0 0 Cell 1\n"
            " CK[1] : CLK.CLK \n"
            " EN[1] : WL.WL \n"
            " R[1] : RST.RST \n"
            " D[1] : BL.BL \n"
            " Q[1] : OC.OC\n\n";
}

/// It prints all the needed .limcad files relative to the needed LiM Cells and the needed .irlcad files
void PrintCadFile::printArray(){

    // iterator over the LiMOperations map
    std::map<std::string, std::string>::const_iterator OpImplIT;
    // string that will contain the type of operation
    std::string opType;
    // if true, an input 2to1 Mux is needed in the LiM cell
    bool isMux;

    // cycling over the LiM array
    for(limArrayIT = ((*compArray).limArray).begin(); limArrayIT != ((*compArray).limArray).end(); ++limArrayIT){

        // get the iterator pointing to the operation of the current row inside the LiMOperations map
        OpImplIT = LimOperations.find((limArrayIT->second).rowType);

        // check if it is a recognized operation
        if(OpImplIT!=LimOperations.end()){
        
            // get the operation type
            opType=OpImplIT->second;

            // memory-only LiM cell
            if(opType == "null"){
                printResultLimcad();
            
            // LiM cell containing a logic opearator and, eventually, an input 2to1 Mux
            }else if(opType == "bitwise"){

                // check if an input 2to1 Mux is required
                isMux = (((limArrayIT->second).inputConnections).size()>2) ? true : false;

                if(isMux){                    
                   printBitwiseMux2to1Limcad((limArrayIT->second).rowType);
                }else if((limArrayIT->second).rowType == "switch"){
                    printMixedLimcad((limArrayIT->second).additionalLogic);
                }else{
                    printBitwiseLimcad((limArrayIT->second).rowType);
                }
            
            // LiM cell associated to an addition operation, that will be carried out in the .irlcad file
            }else if(opType == "arith"){

                // check if an input 2to1 Mux is required
                isMux = (((limArrayIT->second).inputConnections).size()>2) ? true : false;

                if(isMux){                    
                    printAddMux2to1Limcad();
                }else{
                    printAddLimcad();
                }
                printAddIrlcadFile();
            }    
        } else {
            llvm_unreachable("Error in PrintCadFile: unknown compiled operation. "
                "The name is not present inside LimOperations map.");
        }
    }
}


/// It prints the .irlcad file, the one describing the intra-row logic
void PrintCadFile::printAddIrlcadFile(){
    // .irlcad file opening with error handling
    std::string irlcadCellName = "LIM_Add.irlcad";
    std::error_code ErrorInfoirlcad;
    irlcadFile = new raw_fd_ostream(irlcadCellName.c_str(), ErrorInfoirlcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfoirlcad) {
        errs() << "Error opening file: " << irlcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the add row .irlcad file.");
    }

    *irlcadFile <<  "ADDER_9 0 0 ADDER 8\n"
                    " A[1] : OC.OC \n"
                    " B[1] : OLiM.OLiM \n"
                    " S[1] : OIRL.OIRL\n"
                    " \n"

                    "OIRL 0 0 Ext 5 Output\n"
                    " OIRL[1] : \n"
                    "WL 0 0 Ext 5 Input\n"
                    " WL[1] : \n"
                    "RST 0 0 Ext 5 Input\n"
                    " RST[1] : \n"
                    "OLiM 0 0 Ext 5 Input\n"
                    " OLiM[1] : \n"
                    "OC 0 0 Ext 5 Input\n"
                    " OC[1] : \n"
                    "BL 0 0 Ext 5 Input\n"
                    " BL[1] : \n"
                    "CLK 0 0 Ext 5 Input\n"
                    " CLK[1] : \n";

    (*irlcadFile).close();
}




/// It prints the second .limcad file, the one describing the memory array
void PrintCadFile::printLimcadMemFile(){
    // seocnd .limcad file opening with error handling
    std::string limcadCellName = "TOP.limcad";
    std::error_code ErrorInfolimcad;
    limcadMemFile = new raw_fd_ostream(limcadCellName.c_str(), ErrorInfolimcad,
                               llvm::sys::fs::F_None);
    if (ErrorInfolimcad) {
        errs() << "Error opening file: " << limcadCellName << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the TOP entity .limcad file");
    }

    *limcadMemFile <<   "uROM_address 0 0 Ext 5 Output\n"
                        " uROM_address[9.0] : \n"
                        "uIreg 0 0 Ext 5 Input\n"
                        " uIreg[26] : \n"
                        "LiMactivate 0 0 Ext 5 Input\n"
                        " LiMactivate[1] : \n"
                        "queueWen 0 0 Ext 5 Input\n"
                        " queueWen[1] : \n"
                        "queueIN 0 0 Ext 5 Input\n"
                        " queueIN[9.0] : \n"
                        "DOUT 0 0 Ext 5 Output\n"
                        " DOUT[10] : \n"
                        "RST 0 0 Ext 5 Input\n"
                        " RST[1] : \n"
                        "WL 0 0 Ext 5 Input\n"
                        " WL[10] : \n"
                        "BL 0 0 Ext 5 Input\n"
                        " BL[10] : \n"
                        "EN 0 0 Ext 5 Input\n"
                        " EN[1] : \n"
                        "CLK 0 0 Ext 5 Input\n"
                        " CLK[1] : \n"
                        " \n"
                        "MemoryInterface_32 0 0 MemoryInterface 11\n"
                        " CLK[1] : CLK.CLK \n"
                        " queueIN[9.0] : queueIN.queueIN \n"
                        " queueWen[1] : queueWen.queueWen \n"
                        " LiMactivate[1] : LiMactivate.LiMactivate \n"
                        " uIreg[26] : uIreg.uIreg \n"
                        " RST[1] : RST.RST \n"
                        " S0[10] : \n"
                        " S1[10] : \n"
                        " uROM_address[9.0] : uROM_address.uROM_address \n"
                        " \n"
                        "MEMORYARRAY_1 0 0 MEMORYARRAY 7 type:2 rows:10 columns:10 n_read_ports:1 n_write_ports:1 n_read_write_ports:0\n"
                        " CLK[1] : CLK.CLK \n"
                        " EN[1] : EN.EN \n"
                        " S0[10] : MemoryInterface_32.S0 \n"
                        " S1[10] : MemoryInterface_32.S1 \n"
                        " BL[10] : BL.BL \n"
                        " WL[10] : WL.WL \n"
                        " RST[1] : RST.RST \n"
                        " DOUT[10] : DOUT.DOUT \n";

    (*limcadMemFile).close();
}




/// It prints the needed .csv file in order to specify the location of each LiM cell type inside the memory array
void PrintCadFile::printLimcadCsvFile(){
    // .csv file opening with error handling
    std::string csvFileName = "LIM_Cell.csv";
    std::error_code ErrorInfoCsv;
    arrayCsvFile = new raw_fd_ostream(csvFileName.c_str(), ErrorInfoCsv,
                               llvm::sys::fs::F_None);
    if (ErrorInfoCsv) {
        errs() << "Error opening file: " << arrayCsvFile << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the .csv file specifying the array.");
    }

    // iterator over the LiMOperations map
    std::map<std::string, std::string>::const_iterator OpImplIT;
    // string that will contain the type of operation
    std::string opType;
    // counter variable
    int i;
    // if true, an input 2to1 Mux is needed in the LiM cell
    bool isMux;

    // cycling over the LiM array
    for(limArrayIT = ((*compArray).limArray).begin(); limArrayIT != ((*compArray).limArray).end(); ++limArrayIT){

        // get the iterator pointing to the operation of the current row inside the LiMOperations map
        OpImplIT = LimOperations.find((limArrayIT->second).rowType);

        // check if it is a recognized operation
        if(OpImplIT!=LimOperations.end()){
        
            // get the operation type
            opType=OpImplIT->second;

            // memory-only LiM cell
            if(opType == "null"){
                for(i = 0; i < par; ++i){
                    *arrayCsvFile << "LIM_Memory_cell, ";
                }
                *arrayCsvFile << "\n";
            
            // LiM cell containing a logic opearator and, eventually, an input 2to1 Mux
            }else if(opType == "bitwise"){

                // check if an input 2to1 Mux is required
                isMux = (((limArrayIT->second).inputConnections).size()>2) ? true : false;

                if(isMux){                    
                    for(i = 0; i < par; ++i){
                        *arrayCsvFile << "LIM_BitwiseMux2to1_cell, ";
                    }
                }else{
                    for(i = 0; i < par; ++i){
                        *arrayCsvFile << "LIM_Bitwise_cell, ";
                    }
                }
                *arrayCsvFile << "\n";
            
            // LiM cell associated to an addition operation, that will be carried out in the .irlcad file
            }else if(opType == "arith"){

                // check if an input 2to1 Mux is required
                isMux = (((limArrayIT->second).inputConnections).size()>2) ? true : false;

                if(isMux){                    
                    for(i = 0; i < par; ++i){
                        *arrayCsvFile << "LIM_MemoryMux2to1_cell, ";
                    }
                }else{
                    for(i = 0; i < par; ++i){
                        *arrayCsvFile << "LIM_Memory_cell, ";
                    }
                }
                *arrayCsvFile << "\n";

            // multiple logic operator LiM cell
            }else if(opType == "switch"){
                for(i = 0; i < par; ++i){
                    *arrayCsvFile << "LIM_BitwiseMixed_cell.limcad, ";
                }
                *arrayCsvFile << "\n";
            }
        } else {
            llvm_unreachable("Error in PrintCadFile: unknown compiled operation. "
                "The name is not present inside LimOperations map.");
        }
    }

}

/// It prints the needed .csv file in order to specify the intra-row logic needed for a LiM row
void PrintCadFile::printIrlcadCsvFile(){
    // .csv file opening with error handling
    std::string csvFileName = "LIM_IRL.csv";
    std::error_code ErrorInfoCsv;
    intraRowCsvFile = new raw_fd_ostream(csvFileName.c_str(), ErrorInfoCsv,
                               llvm::sys::fs::F_None);
    if (ErrorInfoCsv) {
        errs() << "Error opening file: " << intraRowCsvFile << "\n";
        llvm_unreachable("Error in PrintVhdlFile: unable to create the .csv file specifying intra-row logic.");
    }

    // cycling over the LiM array
    for(limArrayIT = ((*compArray).limArray).begin(); limArrayIT != ((*compArray).limArray).end(); ++limArrayIT){

        // check if the row is an add/sub one
        if((limArrayIT->second).rowType == "add" || (limArrayIT->second).rowType == "sub"){
            // inserting the intra-row logic
            *intraRowCsvFile << "LIM_Add, ";
        }else{
            // inserting nothing
            *intraRowCsvFile << " , ";
        }

    }
}