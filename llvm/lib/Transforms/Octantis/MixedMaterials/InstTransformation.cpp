/*-------------------------------------- The Octantis Project --------------------------------------*/
/// \file
/// InstTransformation Pass: It replaces the common boolean expressions with more efficient ones.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

/*-------------------------------------- The Octantis Project --------------------------------------*/
/// \file
/// OctantisPass Pass: backend pass for the generation of Dexima's configuration files.
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"


#include "InstCombineInternal.h"
#include "llvm/Analysis/CmpInstAnalysis.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/IR/ConstantRange.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/PatternMatch.h"
using namespace llvm;
using namespace PatternMatch;

namespace octantis{

  // OctantisPass
  struct IntTransformationPass : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    IntTransformationPass() : FunctionPass(ID) {}

    // SCHEDULING
    bool runOnFunction(Function &F) override {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {

                 *Op1 = I.getOperand(1);

                If (match(m_Not(m_Xor(m_Value(A), m_Value(B)))))
                {
                    return CreateXnor(A, B);
                }
            }
        }
      return false;
    }

  private:
    Value *Op1;
    Value *A, *B;

  };


}// End Octantis' namespace


using namespace octantis;

char IntTransformationPass::ID = 0;
static RegisterPass<IntTransformationPass> X("intTransfPass", "The pass simplifies some input boolean patterns",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

////


//def XNORrr : F3_1<2, 0b000111,
//    (outs IntRegs:$dst), (ins IntRegs:$b, IntRegs:$c),
//    "xnor $b, $c, $dst",
//    [(set i32:$dst, (not (xor i32:$b, i32:$c)))]>;


