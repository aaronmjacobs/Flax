#ifndef FLAX_POSIX_FIBER_IMPL_H
#define FLAX_POSIX_FIBER_IMPL_H

#include "Flax/Common.h"

namespace flax {

class PosixFiberImpl {
public:
   PosixFiberImpl(Fiber* fiber, FiberMainFunction mainFunction, bool isMainFiber) {
   }

   ~PosixFiberImpl() {
   }

   bool isValid() const {
      return false;
   }

   void pause() {
   }

   void resume() {
   }
};

} // namespace flax

#endif
