/*-------------------------------------- The Octantis Project --------------------------------------*/
//
//AdditionalLogicPorts class: It extends the LLVM IR language introducing all the logic ports
//                             available in the RTL design.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef ADDITIONALLOGICPORTS_H
#define ADDITIONALLOGICPORTS_H

//Standard C++ include files
#include <map>
#include <iterator>
#include <string>


namespace octantis{

///It extends the LLVM IR language introducing all the logic ports
/// available in the RTL design.
class AdditionalLogicPorts
{
public:
    AdditionalLogicPorts();

    ///It returns the corresponding negative logic
    std::string getNegativeLogic(std::string logic);

private:

    ///Associative map between standard logic and negative logic
    const std::map<std::string,std::string> negativeLogic={
        {"and","nand"},
        {"or","nor"},
        {"xor","xnor"}
    };

    ///Iterator over the negativeLogic map
    std::map<std::string,std::string>::const_iterator negativeLogicIT;

};

} //End Octantis' namespace

#endif // ADDITIONALLOGICPORTS_H
