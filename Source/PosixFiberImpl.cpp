#include "Flax/PosixFiberImpl.h"

#include <sys/resource.h>

#include <cassert>

namespace flax {

namespace {

std::size_t getStackSize() {
   static std::size_t stackSize = 0;

   if (stackSize == 0) {
      rlimit limit = {};
      getrlimit(RLIMIT_STACK, &limit);

      stackSize = static_cast<std::size_t>(limit.rlim_cur);
   }

   return stackSize;
}

void fiberStart(FiberAndMain* fiberAndMain) {
   fiberAndMain->mainFunction(fiberAndMain->fiber);
}

} // namespace

PosixFiberImpl::PosixFiberImpl(Fiber* fiber, FiberMainFunction mainFunction, bool isMainFiber)
   : context{}, stack(nullptr), valid(true), fiberAndMain(fiber, mainFunction) {
   valid = (getcontext(&context) == 0);

   if (valid && !isMainFiber) {
      std::size_t stackSize = getStackSize();
      stack = std::unique_ptr<uint8_t[]>(new uint8_t[stackSize]);

      context.uc_stack.ss_sp = stack.get();
      context.uc_stack.ss_size = stackSize;
      context.uc_link = nullptr;

      makecontext(&context, reinterpret_cast<void (*)()>(&fiberStart), 1, &fiberAndMain);
   }
}

PosixFiberImpl::~PosixFiberImpl() {
}

void PosixFiberImpl::swapTo(PosixFiberImpl& lastFiberImpl) {
   assert(valid && lastFiberImpl.valid);
   bool success = (swapcontext(&lastFiberImpl.context, &context) == 0);
   assert(success);
}

} // namespace flax
