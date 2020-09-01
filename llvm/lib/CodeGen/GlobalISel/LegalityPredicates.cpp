//===- lib/CodeGen/GlobalISel/LegalizerPredicates.cpp - Predicates --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A library of predicate factories to use for LegalityPredicate.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

using namespace llvm;

LegalityPredicate LegalityPredicates::typeIs(unsigned TypeIdx, LLT Type) {
  return
      [=](const LegalityQuery &Query) { return Query.Types[TypeIdx] == Type; };
}

LegalityPredicate
LegalityPredicates::typeInSet(unsigned TypeIdx,
                              std::initializer_list<LLT> TypesInit) {
  SmallVector<LLT, 4> Types = TypesInit;
  return [=](const LegalityQuery &Query) {
    return std::find(Types.begin(), Types.end(), Query.Types[TypeIdx]) != Types.end();
  };
}

LegalityPredicate LegalityPredicates::typePairInSet(
    unsigned TypeIdx0, unsigned TypeIdx1,
    std::initializer_list<std::pair<LLT, LLT>> TypesInit) {
  SmallVector<std::pair<LLT, LLT>, 4> Types = TypesInit;
  return [=](const LegalityQuery &Query) {
    std::pair<LLT, LLT> Match = {Query.Types[TypeIdx0], Query.Types[TypeIdx1]};
    return std::find(Types.begin(), Types.end(), Match) != Types.end();
  };
}

LegalityPredicate LegalityPredicates::typePairAndMemDescInSet(
    unsigned TypeIdx0, unsigned TypeIdx1, unsigned MMOIdx,
    std::initializer_list<TypePairAndMemDesc> TypesAndMemDescInit) {
  SmallVector<TypePairAndMemDesc, 4> TypesAndMemDesc = TypesAndMemDescInit;
  return [=](const LegalityQuery &Query) {
    TypePairAndMemDesc Match = {Query.Types[TypeIdx0], Query.Types[TypeIdx1],
                                Query.MMODescrs[MMOIdx].SizeInBits,
                                Query.MMODescrs[MMOIdx].AlignInBits};
    return std::find_if(
      TypesAndMemDesc.begin(), TypesAndMemDesc.end(),
      [=](const TypePairAndMemDesc &Entry) ->bool {
        return Match.isCompatible(Entry);
      }) != TypesAndMemDesc.end();
  };
}

LegalityPredicate LegalityPredicates::isScalar(unsigned TypeIdx) {
  return [=](const LegalityQuery &Query) {
    return Query.Types[TypeIdx].isScalar();
  };
}

LegalityPredicate LegalityPredicates::isVector(unsigned TypeIdx) {
  return [=](const LegalityQuery &Query) {
    return Query.Types[TypeIdx].isVector();
  };
}

LegalityPredicate LegalityPredicates::isPointer(unsigned TypeIdx) {
  return [=](const LegalityQuery &Query) {
    return Query.Types[TypeIdx].isPointer();
  };
}

LegalityPredicate LegalityPredicates::isPointer(unsigned TypeIdx,
                                                unsigned AddrSpace) {
  return [=](const LegalityQuery &Query) {
    LLT Ty = Query.Types[TypeIdx];
    return Ty.isPointer() && Ty.getAddressSpace() == AddrSpace;
  };
}

LegalityPredicate LegalityPredicates::narrowerThan(unsigned TypeIdx,
                                                   unsigned Size) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return QueryTy.isScalar() && QueryTy.getSizeInBits() < Size;
  };
}

LegalityPredicate LegalityPredicates::widerThan(unsigned TypeIdx,
                                                unsigned Size) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return QueryTy.isScalar() && QueryTy.getSizeInBits() > Size;
  };
}

LegalityPredicate LegalityPredicates::scalarOrEltNarrowerThan(unsigned TypeIdx,
                                                              unsigned Size) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return QueryTy.getScalarSizeInBits() < Size;
  };
}

LegalityPredicate LegalityPredicates::scalarOrEltWiderThan(unsigned TypeIdx,
                                                           unsigned Size) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return QueryTy.getScalarSizeInBits() > Size;
  };
}

LegalityPredicate LegalityPredicates::scalarOrEltSizeNotPow2(unsigned TypeIdx) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return !isPowerOf2_32(QueryTy.getScalarSizeInBits());
  };
}

LegalityPredicate LegalityPredicates::sizeNotPow2(unsigned TypeIdx) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return QueryTy.isScalar() && !isPowerOf2_32(QueryTy.getSizeInBits());
  };
}

LegalityPredicate LegalityPredicates::sizeIs(unsigned TypeIdx, unsigned Size) {
  return [=](const LegalityQuery &Query) {
    return Query.Types[TypeIdx].getSizeInBits() == Size;
  };
}

LegalityPredicate LegalityPredicates::sameSize(unsigned TypeIdx0,
                                               unsigned TypeIdx1) {
  return [=](const LegalityQuery &Query) {
    return Query.Types[TypeIdx0].getSizeInBits() ==
           Query.Types[TypeIdx1].getSizeInBits();
  };
}

LegalityPredicate LegalityPredicates::memSizeInBytesNotPow2(unsigned MMOIdx) {
  return [=](const LegalityQuery &Query) {
    return !isPowerOf2_32(Query.MMODescrs[MMOIdx].SizeInBits / 8);
  };
}

LegalityPredicate LegalityPredicates::numElementsNotPow2(unsigned TypeIdx) {
  return [=](const LegalityQuery &Query) {
    const LLT QueryTy = Query.Types[TypeIdx];
    return QueryTy.isVector() && !isPowerOf2_32(QueryTy.getNumElements());
  };
}

LegalityPredicate LegalityPredicates::atomicOrderingAtLeastOrStrongerThan(
    unsigned MMOIdx, AtomicOrdering Ordering) {
  return [=](const LegalityQuery &Query) {
    return isAtLeastOrStrongerThan(Query.MMODescrs[MMOIdx].Ordering, Ordering);
  };
}