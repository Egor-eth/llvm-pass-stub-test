#include <algorithm>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
struct FNamePrinter : public FunctionPass {
  static char ID;
  FNamePrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    int maxOp = 0;

    for(const BasicBlock &blk : F) {
      for(const Instruction &ins : blk) {
        if(ins.getOpcode() == Instruction::Call) {
          const CallInst *callInst = cast<CallInst>(&ins);
          if(callInst->getCalledFunction()->isIntrinsic()) {
            auto optionalOpBundle = callInst->getOperandBundle("gc-live");
            if(optionalOpBundle.hasValue()) {
              const size_t ptrOps = std::count_if(optionalOpBundle->Inputs.begin(), optionalOpBundle->Inputs.end(), [](const Use &use) {
                return use->getType()->isPointerTy();
              });
              maxOp = ptrOps > maxOp ? ptrOps : maxOp;
            }
          }
        }
      }
    }
    errs() << "Maximum number of statepoint's operands is " << maxOp << "\n";
    return false;
  }
}; // end of struct FNamePrinter
}  // end of anonymous namespace

char FNamePrinter::ID = 0;
static RegisterPass<FNamePrinter> X("fname_printer", "FNamePrinter Pass",
                                   false /* Only looks at CFG */,
                                   true /* Analysis Pass */);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new FNamePrinter()); });
