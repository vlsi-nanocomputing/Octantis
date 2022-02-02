/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LimArray Class: it implements the necessary structures to model the LiM Unit
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
// © Alessio Nicola 2021 (alessio.nicola@studenti.polito.it) for Politecnico di Torino
// 
/*--------------------------------------------------------------------------------------------------*/
#include "LiMArray.h"

//Octantis Include Files
#include "OperationsImplemented.h"

//LLVM Include Files
#include "llvm/Support/raw_ostream.h"

using namespace octantis;
using namespace llvm;


LiMArray::LiMArray()
{

}

///Function to add a new memory Row inside the array
///      NOTEs: The type is not essential, its default value should be "load"
void LiMArray::addNewRow(int* const &rowName, std::string &rowType, int &rowLength){

    LiMRow tmpRow;
    tmpRow.rowType=rowType;
    tmpRow.rowLength=rowLength;

    // The first element of the list is dedicated only for Memory input
    // 0xDEF => Cell with NO memory input
    // It is convenient to distinguish from real memory input cell (multiple of 4)
    tmpRow.inputConnections.push_back((int*)0xdef);

    tmpRow.inputConnectionsType.push_back("none");

    limArray.insert({rowName,tmpRow});

}

///Function to add a new LiM Row inside the array
void LiMArray::addNewLiMRow(int* const &rowName, std::string &rowType, std::list<std::string> &addLogic, int &rowLength, int* const &src, std::string const &conType){

    LiMRow tmpRow;
    tmpRow.rowType=rowType;
    tmpRow.rowLength=rowLength;

    if(!addLogic.empty())
    {
        errs()<< "Lim compiler: additional logic list not empty!\n";
        tmpRow.additionalLogic=addLogic;
    }

    tmpRow.inputConnections.push_back(src);

    tmpRow.inputConnectionsType.push_back(conType);

    limArray.insert({rowName,tmpRow});
}

///Function to add a new Row inside the array: partial result row
///      NOTEs: The "load" type has to be defined somewhere else
void LiMArray::addNewResultRow(int* const &rowName, int &rowLength, int* const &src, std::string const &conType){

    LiMRow tmpRow;
    tmpRow.rowType="load";
    tmpRow.rowLength=rowLength;
    tmpRow.inputConnections.push_back(src);
    tmpRow.inputConnectionsType.push_back(conType);
    limArray.insert({rowName,tmpRow});

}

///Function to change the type of a LiM row: if a row is just defined as LiM
/// it returns "false" and nothing changes.
bool LiMArray::changeLiMRowType(int* const &rowName, std::string &newRowType, std::list<std::string> &additionalOperators){

    ///Iterator over LiM Array
    std::map<int * const, LiMRow>::iterator limArrayIntIT=findRow(rowName);

    ///Check if the memory row is Lim or not (The iterator is constant)
    if(!checkIfRowIsLiM(limArrayIntIT)){

        ///Check if the row represents an input line
        if(!(limArrayIntIT->second.additionalLogic).empty()){

            std::list<std::string>::iterator specs;
            //Note: the name "additionaLogic" has to be updated to avoid misunderstandings!
            specs=find((limArrayIntIT->second.additionalLogic).begin(),(limArrayIntIT->second.additionalLogic).end(),"input_line");

            if(specs!=(limArrayIntIT->second.additionalLogic).end()){
                    return false;
            }
        }

        //errs() << "\t\t" << rowName << " is of type " << limArrayIntIT->second.rowType << "\n";
        limArrayIntIT->second.rowType=(newRowType);
        //errs() << "\t\t"  << rowName << " becomes " <<limArrayIntIT->second.rowType << "\n";

        ///Check the presence of additional operators
        if(!additionalOperators.empty())
        {
            limArrayIntIT->second.additionalLogic=additionalOperators;
        }
        return true;
    } else {
        return false;
    }
}


///Function to check if a memory row is LiM or not
///  NOTEs: this has to be updated to remove the "load" lable!
bool LiMArray::checkIfRowIsLiM(std::map<int * const, LiMArray::LiMRow>::iterator &limArrayItpt){

    ///Check if the the Row is type "load"
    return ((limArrayItpt->second.rowType)!= "load") ? true : false;
}


///Function to find element inside the limArray
std::map<int * const, LiMArray::LiMRow>::iterator LiMArray::findRow(int* const &rowName){

    limArrayIT=limArray.find(rowName);

    if(limArrayIT!=limArray.end())
    {
        //The row is present inside the array
        return limArrayIT;

    } else {
        //An error occurred in finding a specific row
        errs()<< "Error in finding " << rowName << "\n\n";
        printLiMArray();
        llvm_unreachable("LiMArray error: Error in finding a row inside the LiMArray.");
    }

}

///Function to add a new input connection to a LiM row if not present
void LiMArray::addNewInputConnection(int* const &rowName, int* const &srcRowName, std::string const &conType){

    //Iterator over LiM Array
    std::map<int * const, LiMRow>::iterator limArrayIntIT=findRow(rowName);

    //Set an alias for the list
    std::list<int *> * connectionsList=&(limArrayIntIT->second.inputConnections);
    std::list<std::string> * conTypeList = &(limArrayIntIT->second.inputConnectionsType);

    //Add the new input connection
    if((*connectionsList).size()==1 || find((*connectionsList).begin(), (*connectionsList).end(),
                                          srcRowName) == (*connectionsList).end()){
        //errs() << "\taddNewInputConnection: source not found!\n";
        (*connectionsList).push_back(srcRowName);
        (*conTypeList).push_back(conType);
    } else {
        //errs() << "\taddNewInputConnection: source found!\n";
    }

}

///Function to add logic inside a specific LiM row
void LiMArray::addLogicToRow(int * const &rowName, std::string &additionalLogicType){

    //Iterator over LiM Array
    std::map<int * const, LiMRow>::iterator limArrayIntIT=findRow(rowName);

    //Check if the row embeds already the shift operation
    //Check in the additional logic list
    std::list<std::string>::iterator specsIt=find((limArrayIntIT->second.additionalLogic).begin(),
                                                  (limArrayIntIT->second.additionalLogic).end(),
                                                  additionalLogicType);
    if(specsIt==(limArrayIntIT->second.additionalLogic).end()){
        (limArrayIntIT->second.additionalLogic).push_back(additionalLogicType);
    }

}

///Function to know if a memory row has the same type requested
bool LiMArray::isLiMRowOfThisType(int* const &rowName, std::string &type){

    //Iterator over LiM Array
    std::map<int * const, LiMRow>::iterator limArrayIntIT=findRow(rowName);

    //Add the new input connection
    return ((limArrayIntIT->second.rowType)==type);

}

///Function to return the dimensions of the LiM Array
int LiMArray::getDimensions(){
    return limArray.size();
}


void LiMArray::deleteLiMRow(int* const &rowName){

    limArrayIT = limArray.find(rowName);
    limArray.erase(limArrayIT);

}
//----------------------------DEBUG FUNCTIONS---------------------------

void LiMArray::printLiMArray(){

    //Iterator over LiM Array
    std::map<int * const, LiMRow>::iterator limArrayDebugIT;

    //Iteratore over the input connection list
    std::list<int *>::iterator inList;

    //Iteratore over the input connection type list
    std::list<std::string>::iterator inTypeList;

    //Iterator over the additional logic list
    std::list<std::string>::iterator inAddList;

    //Current LiM row
    int rowCount=0;

    int addN = 0;
    int loadN = 0;
    int sdivN = 0;
    int sXor = 0;
    int nMux = 0;

    //Check if the map is empty:
    if(limArray.size()==0)
    {
        errs()<< "\tERROR! The input map is empty!";
    } else {
        for(limArrayDebugIT=limArray.begin(); limArrayDebugIT!=limArray.end();++limArrayDebugIT){

            if(((limArrayDebugIT->second).inputConnections).size() == 3){
                nMux++;
            }

            errs() << "Row " << rowCount << ", name " << limArrayDebugIT->first << ": Type " << (limArrayDebugIT->second).rowType
                   << ", length " << (limArrayDebugIT->second).rowLength << " additional integrated logic ";

            if((limArrayDebugIT->second).rowType == "add"){
                addN++;
            }else if((limArrayDebugIT->second).rowType == "load"){
                loadN++;
            }else if((limArrayDebugIT->second).rowType == "sdiv"){
                sdivN++;
            }else if((limArrayDebugIT->second).rowType == "xor"){
                sXor++;
            }

            if(((limArrayDebugIT->second).additionalLogic).empty())
            {
                errs() << "NULL, ";
            } else {

              for(inAddList=((limArrayDebugIT->second).additionalLogic).begin();
                  inAddList!=((limArrayDebugIT->second).additionalLogic).end(); ++inAddList)
              {
                  errs() << *inAddList << ", ";
              }

            }


            errs() << "\ninputs\t";
            for(inList=((limArrayDebugIT->second).inputConnections).begin();inList!=((limArrayDebugIT->second).inputConnections).end(); ++inList){
                errs() << *inList;
                if(inList == ((limArrayDebugIT->second).inputConnections).begin())
                    errs() << " \t";
                else
                    errs() << " ";
            }

            errs() << "\ntype  \t";
            for(inTypeList=((limArrayDebugIT->second).inputConnectionsType).begin();inTypeList!=((limArrayDebugIT->second).inputConnectionsType).end(); ++inTypeList){
                errs() << *inTypeList;
                if(inTypeList == ((limArrayDebugIT->second).inputConnectionsType).begin())
                    errs() << " \t";
                else
                    errs() << " ";
            }

            errs() << "\n";
            ++rowCount;

        }

        errs() << addN << " add rows, " << loadN << " load rows, " << sdivN << " sdiv rows " << sXor << " xor rows\n";
        errs() << nMux << " rows with muxes\n";
    }

}

//------------------------END DEBUG FUNCTIONS---------------------------
