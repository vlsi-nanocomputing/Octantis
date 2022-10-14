/*-------------------------------------- The Octantis Project --------------------------------------*/
//
//OperationsDelay class: It contains the high-level delays associated to each operation
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef OPERATIONSDELAY_H
#define OPERATIONSDELAY_H

//Standard C++ include files
#include <map>
#include <iterator>
#include <string>


namespace octantis{

///It extends the LLVM IR language introducing all the logic ports
/// available in the RTL design.
class OperationsDelay
{
public:
    OperationsDelay(){};

    ///It returns the corresponding delay
    int getOperationDelay(std::string operation);

private:

    ///Associative map between standard operations and delay
    const std::map<std::string, int> delayMap = {
        {"and", 1},
        {"nand", 1},
        {"or", 1},
        {"nor", 1},
        {"xor", 1},
        {"xnor", 1},
        {"add", 1},
        {"sub", 1},
        {"mul", 1},
    };

    ///Iterator over the negativeLogic map
    std::map<std::string, int>::const_iterator delayMapIT;

};

} //End Octantis' namespace

#endif // OPERATIONSDELAY_H
