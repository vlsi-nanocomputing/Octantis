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
#include <map>
#include <set>

namespace octantis {

//Place here the list of operations (in LLVM IR form, but uppercase)
//that Octantis will recognize as LiM Operations with the associated type
//      *TYPES: "null" for LOAD OPERATIONS
//              "arith" for ARITHMETIC OPERATIONS
//              "bitwise" for BITWISE OPERATIONS
const std::map<std::string, std::string> LimOperations = {
    {"load", "null"},
    {"add", "arith"},
    {"sub", "arith"},
    {"and", "bitwise"},
    {"nand", "bitwise"},
    {"or", "bitwise"},
    {"xor", "bitwise"}
};

//Place here the list of elements that Octantis
//will recognize as LiM Operations
const std::set<std::string> PeriperalOperations = {
    "mux",
    "popcount",
    "upcount"
};



///-------------------OLD MATERIAL----------------------

// Allowed Dexima Bitwise instructions
//const std::set<std::string> BitwiseOpCode = {
//    //Bitwise binary operations
//    "And",
//    "Or",
//    "Xor",
//    //"Shl",//Left shift
//    //"Shr",//Right shift
//    //Comparison Operations
//    //"Eq",
//    //"Ne",
//    //"Le",
//    //"Ge",
//    //"Lt",
//    //"Gt"
//};


// Allowed Dexima Arithmetic instructions
//const std::set<std::string> ArithOpCode = {
//    //Binary operations
//    "Add",
//    "AddMux",
//    "Sub",
//    "SubMux",
//    //"Mul",
//    //"Div",
//    //"Rem",
//    //"Shl",//Left shift
//    //"Shr",//Right shift
//};

// Allowed Dexima Comparison instructions: NOT IMPLEMENTED
//    const std::set<std::string> CompOpCode = {
//        "Eq",
//        "Ne",
//        "Le",
//        "Ge",
//        "Lt",
//        "Gt"
//    };

///-----------------------------------------------------

}

#endif // OPERATIONSIMPLEMENTED_H
