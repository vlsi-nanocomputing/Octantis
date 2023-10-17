/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// AdditionalLogicPorts class: It extends the LLVM IR language introducing all the logic ports
//                             available in the RTL design.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "AdditionalLogicPorts.h"

//Include files of LLVM
#include "llvm/Support/raw_ostream.h"

using namespace octantis;

AdditionalLogicPorts::AdditionalLogicPorts()
{

}

///It returns the corresponding negative logic
std::string AdditionalLogicPorts::getNegativeLogic(std::string logic){

    negativeLogicIT=negativeLogic.find(logic);

    if(negativeLogicIT!=negativeLogic.end())
    {
        return negativeLogicIT->second;

    } else {
        //An error occurred
        llvm_unreachable("Error in AdditionalLogicPorts: The input logic port is unknown.");

    }

}
