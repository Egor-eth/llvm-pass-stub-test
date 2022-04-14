#include <algorithm>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Statepoint.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

enum {DEFAULT_MAX = 2};

cl::opt<unsigned> MaxGCLiveArgs("m", cl::desc("Specify maximum arguments for check"), cl::value_desc("count"), cl::init(DEFAULT_MAX));

struct StatePointChecker : public FunctionPass {
  static char ID;
  StatePointChecker() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {

    const unsigned MaxNum = MaxGCLiveArgs.getValue();

    for(const BasicBlock &B : F) {
      for(const Instruction &I : B) {
        const GCStatepointInst *StatepointInst = dyn_cast<GCStatepointInst>(&I);
        if(StatepointInst) {
          const size_t PtrOps = std::count_if(StatepointInst->gc_args_begin(),
            StatepointInst->gc_args_end(), 
            [](const Use &U) {
              return U->getType()->isPointerTy();
            });
          if(PtrOps > MaxNum) {
            errs() 
              << "[Statepoint's checker] "
              << "Warning: maximum number of statepoint's operands is more than " 
              << MaxNum << ": " << I << "\n";
          }
        }
      }
    }
    return false;
  }
}; // end of struct StatePointChecker
}  // end of anonymous namespace

char StatePointChecker::ID = 0;
static RegisterPass<StatePointChecker> X("statepoint_checker", "Statepoint's checker Pass",
                                   false /* Only looks at CFG */,
                                   false /* Analysis Pass */);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new StatePointChecker()); });
