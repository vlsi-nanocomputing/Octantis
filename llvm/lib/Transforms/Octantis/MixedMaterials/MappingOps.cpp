/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// MappingOps: Class useful for the correct mapping between LLVM IR and Dexima config. files
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "MappingOps.h"

using namespace llvm;
using namespace octantis;

namespace octantis {

MappingOps::MappingOps(Instruction *instruct){
    if(isa<BinaryOperator>(instruct)){
        switch(instruct->getOpcode()){
            case Instruction::Add: operation = Add; break;
            case Instruction::Sub: operation = Sub; break;
            case Instruction::Mul: operation = Mul; break;
            case Instruction::UDiv: operation = Div; break;
            case Instruction::URem: operation = Rem; break;
            case Instruction::And: operation = And; break;
            case Instruction::Or: operation = Or; break;
            case Instruction::Xor: operation = Xor; break;
            case Instruction::Shl: operation = Shl; break;
            case Instruction::Shr: operation = Shr; break;
            default: llvm_unreachable("Operator not valid!");
        }
    } else if(const ICmpInst *comparison = dyn_cast<ICmpInst>(instruct)){
        switch (comparison->getPredicate()){
            case ICmpInst::ICMP_EQ: operation = Eq; break;
            case ICmpInst::ICMP_NE: operation = Ne; break;
            case ICmpInst::ICMP_ULE: operation = Le; break;
            case ICmpInst::ICMP_UGE: operation = Ge; break;
            case ICmpInst::ICMP_ULT: operation = Lt; break;
            case ICmpInst::ICMP_UGT: operation = Gt; break;
        default: llvm_unreachable("ICmp operator not valid!");
        }
    } else {
        errs () << "Unknown instruction: " << *instruct << "\n";
        llvm_unreachable(0);
    }
}
}

