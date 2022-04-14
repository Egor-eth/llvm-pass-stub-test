#include <algorithm>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Statepoint.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

struct StatePointCounter : public FunctionPass {
  static char ID;
  StatePointCounter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    int MaxOp = 0;

    for(const BasicBlock &B : F) {
      for(const Instruction &I : B) {
        const GCStatepointInst *StatepointInst = dyn_cast<GCStatepointInst>(&I);
        if(StatepointInst) {
          const size_t PtrOps = std::count_if(StatepointInst->gc_args_begin(),
            StatepointInst->gc_args_end(),
            [](const Use &U) {
              return U->getType()->isPointerTy();
            });
          MaxOp = PtrOps > MaxOp ? PtrOps : MaxOp;
        }
      }
    }
    errs() << "[Statepoint's count] " << F.getName() 
      << ": Maximum number of statepoint's operands is " << MaxOp << "\n";
    return false;
  }
}; // end of struct StatePointCounter
}  // end of anonymous namespace

char StatePointCounter::ID = 0;
static RegisterPass<StatePointCounter> X("statepoint_counter", "Statepoint's Pass",
                                   false /* Only looks at CFG */,
                                   false /* Analysis Pass */);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new StatePointCounter()); });
