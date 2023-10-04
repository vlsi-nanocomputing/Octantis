/*-------------------------------------- The Octantis Project --------------------------------------*/
/// 
/// OperationsDelay class: It contains the high-level delays associated to each operation
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
// © Alessio Nicola 2021 (alessio.nicola@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

//LLVM Include Files
#include "llvm/IR/Function.h"

#include "OperationsDelay.h"

#include <string>
#include <map>
#include <set>

using namespace llvm;
using namespace octantis;

///It returns the corresponding delay
int OperationsDelay::getOperationDelay(std::string operation) {

    delayMapIT = delayMap.find(operation);

    int retDelay = -1;

    if (delayMapIT != delayMap.end()) {

        retDelay = delayMapIT->second;

    }

    return retDelay;
}
