#ifndef FLAX_WINDOWS_FIBER_IMPL_H
#define FLAX_WINDOWS_FIBER_IMPL_H

#include "Flax/Common.h"

namespace flax {

class WindowsFiberImpl {
public:
   WindowsFiberImpl(Fiber* fiber, FiberMainFunction mainFunction, bool isMainFiber);

   ~WindowsFiberImpl();

   bool isValid() const {
      return address != nullptr;
   }

   void pause();

   void resume();

private:
   void* address;
   bool mainFiber;
   FiberAndMain fiberAndMain;
};

} // namespace flax

#endif
