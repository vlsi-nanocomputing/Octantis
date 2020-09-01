/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// MappingOps: Class useful for the correct mapping between LLVM IR and Dexima config. files
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef MAPPINGOPS_H
#define MAPPINGOPS_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include <map>

using namespace llvm;

namespace octantis {

/// MappingOps: Any allowed operation is mapped to
/// Dexima input code.
class MappingOps
{
public:

    /// Allowed Dexima instructions
    enum OpCode {
        //Binary operations
        Add,
        Sub,
        Mul,
        Div,
        Rem,
        //Bitwise binary operations
        And,
        Or,
        Xor,
        Shl,//Left shift
        Shr,//Right shift
        //Comparison Operations
        Eq,
        Ne,
        Le,
        Ge,
        Lt,
        Gt
    };


    MappingOps(Instruction *instruct);
    MappingOps();

private:
    Instruction *Inst;
    OpCode operation;
    //Map between operations
};
}
#endif // MAPPINGOPS_H
