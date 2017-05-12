#ifndef FLAX_POSIX_FIBER_IMPL_H
#define FLAX_POSIX_FIBER_IMPL_H

#include "Flax/Common.h"

#define _XOPEN_SOURCE
#include <ucontext.h>

#include <cstdint>
#include <memory>

namespace flax {

class PosixFiberImpl {
public:
   PosixFiberImpl(Fiber* fiber, FiberMainFunction mainFunction, bool isMainFiber);

   ~PosixFiberImpl();

   bool isValid() const {
      return valid;
   }

   static void swap(PosixFiberImpl& from, PosixFiberImpl& to);

private:
   ucontext_t context;
   std::unique_ptr<uint8_t[]> stack;
   bool valid;
   FiberAndMain fiberAndMain;
};

} // namespace flax

#endif
