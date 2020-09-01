/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// Operations Implemented Include File: it includes all the macto-operations recognized by DExIMA
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef OPERATIONSIMPLEMENTED_H
#define OPERATIONSIMPLEMENTED_H

#include <string>
#include <set>

namespace octantis {

//Place here the list of operations (in LLVM IR form, but uppercase)
//that Octantis will recognize as LiM Operations
const std::set<std::string> LimOperations = {
    "add",
    "sub",
    "and",
    "nand",
    "or",
    "xor"
};

//Place here the list of elements that Octantis
//will recognize as LiM Operations
const std::set<std::string> PeriperalOperations = {
    "mux",
    "popcount",
    "upcount"
};

}

#endif // OPERATIONSIMPLEMENTED_H
