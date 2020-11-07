#include "HI_LoopUnroll.h"
#include "HI_print.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <ios>
#include <stdio.h>
#include <stdlib.h>
#include <string>

LoopUnrollResult HI_LoopUnroll::tryToUnrollLoop(
    Loop *L, DominatorTree &DT, LoopInfo *LI, ScalarEvolution &SE, const TargetTransformInfo &TTI,
    AssumptionCache &AC, OptimizationRemarkEmitter &ORE, bool PreserveLCSSA, int OptLevel,
    bool OnlyWhenForced, int ProvidedCount, Optional<unsigned> ProvidedThreshold,
    Optional<bool> ProvidedAllowPartial, Optional<bool> ProvidedRuntime,
    Optional<bool> ProvidedUpperBound, Optional<bool> ProvidedAllowPeeling)
{

    (*LoopUnrollLog << "Loop Unrolling: F[" << L->getHeader()->getParent()->getName() << "] Loop %"
                    << L->getHeader()->getName() << " with factor=" << ProvidedCount << "\n");
    TransformationMode TM = hasUnrollTransformation(L);
    LoopUnrollLog->flush();

    if (TM & TM_Disable)
        return LoopUnrollResult::Unmodified;
    if (!L->isLoopSimplifyForm())
    {
        (*LoopUnrollLog << "  Not unrolling loop which is not in loop-simplify form.\n");
        return LoopUnrollResult::Unmodified;
    }

    // When automtatic unrolling is disabled, do not unroll unless overridden for
    // this loop.
    if (OnlyWhenForced && !(TM & TM_Enable))
        return LoopUnrollResult::Unmodified;

    unsigned NumInlineCandidates;
    bool NotDuplicatable;
    bool Convergent;
    TargetTransformInfo::UnrollingPreferences UP = gatherUnrollingPreferences(
        L, SE, TTI, OptLevel, ProvidedThreshold, ProvidedCount, ProvidedAllowPartial,
        ProvidedRuntime, ProvidedUpperBound, ProvidedAllowPeeling);
    // Exit early if unrolling is disabled.
    if (UP.Threshold == 0 && (!UP.Partial || UP.PartialThreshold == 0))
        return LoopUnrollResult::Unmodified;

    SmallPtrSet<const Value *, 32> EphValues;
    CodeMetrics::collectEphemeralValues(L, &AC, EphValues);

    unsigned TripCount = SE.getSmallConstantMaxTripCount(L);

    // Save loop properties before it is transformed.
    MDNode *OrigLoopID = L->getLoopID();

    // Unroll the loop.
    Loop *RemainderLoop = nullptr;
    // LoopUnrollResult UnrollResult = UnrollLoop(
    //     L, UP.Count, TripCount, UP.Force, UP.Runtime, UP.AllowExpensiveTripCount,
    //     UseUpperBound, MaxOrZero, TripMultiple, UP.PeelCount, UP.UnrollRemainder,
    //     LI, &SE, &DT, &AC, &ORE, PreserveLCSSA, &RemainderLoop);

    Function *recordFunc =
        L->getHeader()->getParent(); // if loop in unrolled completely, the L will be missed.

    LoopUnrollResult UnrollResult =
        UnrollLoop(L, ProvidedCount, TripCount, true, false, true, false, false, 1, 0, true, LI,
                   &SE, &DT, &AC, &ORE, PreserveLCSSA, &RemainderLoop);

    if (UnrollResult == LoopUnrollResult::Unmodified)
    {
        (*LoopUnrollLog
         << "  Not unrolling loop with UnrollResult == LoopUnrollResult::Unmodified.\n");
        return LoopUnrollResult::Unmodified;
    }

    *LoopUnrollLog << "\n\n Functiion AFTER UNROLL \n\n" << *(recordFunc) << "\n";

    LoopUnrollLog->flush();
    if (RemainderLoop)
    {
        *LoopUnrollLog << "\n\n Loop Unroll remainder \n\n" << *RemainderLoop << "\n";
        Optional<MDNode *> RemainderLoopID = makeFollowupLoopID(
            OrigLoopID, {LLVMLoopUnrollFollowupAll, LLVMLoopUnrollFollowupRemainder});
        if (RemainderLoopID.hasValue())
            RemainderLoop->setLoopID(RemainderLoopID.getValue());
    }

    if (UnrollResult != LoopUnrollResult::FullyUnrolled)
    {
        Optional<MDNode *> NewLoopID = makeFollowupLoopID(
            OrigLoopID, {LLVMLoopUnrollFollowupAll, LLVMLoopUnrollFollowupUnrolled});
        if (NewLoopID.hasValue())
        {
            L->setLoopID(NewLoopID.getValue());

            // Do not setLoopAlreadyUnrolled if loop attributes have been specified
            // explicitly.
            return UnrollResult;
        }
    }

    // If loop has an unroll count pragma or unrolled by explicitly set count
    // mark loop as unrolled to prevent unrolling beyond that requested.
    // If the loop was peeled, we already "used up" the profile information
    // we had, so we don't want to unroll or peel again.
    if (UnrollResult != LoopUnrollResult::FullyUnrolled && (true || UP.PeelCount))
        L->setLoopAlreadyUnrolled();
    LoopUnrollLog->flush();
    return UnrollResult;
}

bool HI_LoopUnroll::runOnLoop(Loop *L, LPPassManager &LPM)
{
    if (skipLoop(L))
        return false;
    std::string tmp_loop_name = L->getHeader()->getParent()->getName();
    tmp_loop_name += "-";
    tmp_loop_name += L->getHeader()->getName();

    // bypass loop without label
    if (IRLoop2LoopLabel.find(tmp_loop_name) == IRLoop2LoopLabel.end())
    {
        llvm::errs() << "bypass loop without label tmp_loop_name=" << tmp_loop_name << "\n";
        assert(false &&
               "bypass loop without label tmp_loop_name. Each loop should has its own label. In "
               "the evaluation test, each loop will be set with label automatically while in other "
               "tests, the loops might need to be set with labels manually.");
        return false;
    }

    // assert(IRLoop2LoopLabel.find(tmp_loop_name) != IRLoop2LoopLabel.end() && "each loop in source
    // code should be labeled!");

    std::string loop_label_name = IRLoop2LoopLabel[tmp_loop_name];

    if (LoopLabel2UnrollFactor.find(loop_label_name) == LoopLabel2UnrollFactor.end())
    {
        // llvm::errs() << "no unroll factor for =" << loop_label_name << "\n";
        return false;
    }

    Function &F = *L->getHeader()->getParent();

    auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
    const TargetTransformInfo &TTI = getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
    auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
    // For the old PM, we can't use OptimizationRemarkEmitter as an analysis
    // pass.  Function analyses need to be preserved across loop transformations
    // but ORE cannot be preserved (see comment before the pass definition).
    OptimizationRemarkEmitter ORE(&F);
    bool PreserveLCSSA = mustPreserveAnalysisID(LCSSAID);

    LoopUnrollResult Result = tryToUnrollLoop(
        L, DT, LI, SE, TTI, AC, ORE, PreserveLCSSA, OptLevel, OnlyWhenForced,
        LoopLabel2UnrollFactor[loop_label_name], ProvidedThreshold, ProvidedAllowPartial,
        ProvidedRuntime, ProvidedUpperBound, ProvidedAllowPeeling);

    if (Result == LoopUnrollResult::FullyUnrolled)
        LPM.markLoopAsDeleted(*L);

    return Result != LoopUnrollResult::Unmodified;
}

char HI_LoopUnroll::ID =
    0; // the ID for pass should be initialized but the value does not matter, since LLVM uses the
       // address of this variable as label instead of its value.
