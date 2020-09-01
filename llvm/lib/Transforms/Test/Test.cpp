//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;



namespace {

    void isPointer(Type *T);

  // Hello - The first implementation, without getAnalysisUsage.
  struct Test : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Test() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        for (BasicBlock &BB : F) {
            errs() << "Basic Block: " << BB.getName() << " size " << BB.size() << "\n";

            for (Instruction &I : BB) {
                errs()<< "Instruction fetched: " << I << "\n";
                if(isa <LoadInst> (I))
                {
                    errs() << "\tLOAD DETECTED: "<< I.getOpcodeName() << "\n";
                    errs()<< "\t\t Return operand: " << &I << "\n"; //The instruction is a Value object and its address corresponds to the return address of the operation
                    errs()<< "\t\t Operand 1: " << I.getOperand(0) << "\n";
                    //isPointer(I.getOperand(0)->getType());
                }
                if(isa <StoreInst> (I)){
                    errs() << "\tSTORE DETECTED: "<< I.getOpcodeName() << "\n";
                    errs()<< "\t\t Operand 1: " << I.getOperand(0) << "\n";
                    errs()<< "\t\t Operand 2: " << I.getOperand(1) << "\n";
                }
                if(isa <BinaryOperator> (I)){
                    errs() << "\tBINARY DETECTED: "<< I.getOpcodeName() << "\n";
                    errs()<< "\t\t Return operand: " << &I << "\n";
                    errs()<< "\t\t Operand 1: " << I.getOperand(0) << "\n";
                    errs()<< "\t\t Operand 2: " << I.getOperand(1) << "\n";
                }
                if(isa<AllocaInst>(I)){
                    errs() << "\tALLOCA DETECTED: "<< I.getOpcodeName() << "\n";
                    errs()<< "\t\t Return operand: " << &I << "\n";
                }


//               if(isa <LoadInst> (I))
//                    errs() << "\tLOAD DETECTED: "<< I.getOpcodeName() << "\n";
//               if(isa <StoreInst> (I))
//                    errs() << "\tSTORE DETECTED: "<< I.getOpcodeName() << "\n";
//               if(isa <BinaryOperator> (I))
//                   errs() << "\tBINARY DETECTED: "<< I.getOpcodeName() << "\n";
//               if(isa<AllocaInst>(I))
//                   errs() << "\tALLOCA DETECTED: "<< I.getOpcodeName() << "\n";

//                //errs() << "\t" << I << ", opcode " <<I.getOpcode() << " opName "<< I.getOpcodeName() << "\n";
//                //errs() << "\t\tIs it unary? A: " << I.isUnaryOp() << "\n";
//                //errs() << "\t\tIs it binary? A: " << I.isBinaryOp() << "\n";
//                //errs() << "\t\t operand1: " << I.getOperand(0) << "\n";
//               // isTerminator(): end of basic block;
            }

        }
      //errs() << "Hello: ";
      //errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };

  void isPointer(Type *T){ //We have not to dereferencing the pointers: the addresses are constant! What changes is the type declared!
      if(isa<PointerType>(T)){
            errs() << "The operand is a Pointer. ";
            errs() << "The real value is: " << T->getPointerTo() << "\n";
      }
  }
}

char Test::ID = 0;
static RegisterPass<Test> X("test", "Test Pass");
