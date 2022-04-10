#include <algorithm>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Statepoint.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

enum {MAX_NUM = 2};

struct StatePointChecker : public FunctionPass {
  static char ID;
  StatePointChecker() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {

    for(const BasicBlock &blk : F) {
      for(const Instruction &ins : blk) {
        const GCStatepointInst *statepointInst = dyn_cast<GCStatepointInst>(&ins);
        if(statepointInst) {
          const size_t ptrOps = std::count_if(statepointInst->gc_args_begin(),
            statepointInst->gc_args_end(), 
            [](const Use &use) {
              return use->getType()->isPointerTy();
            });
          if(ptrOps > MAX_NUM) {
            errs() 
              << "[Statepoint's checker] "
              << "Warning: maximum number of statepoint's operands is more than " 
              << MAX_NUM << ": " << ins << "\n";
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
