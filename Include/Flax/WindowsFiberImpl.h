#ifndef FLAX_WINDOWS_FIBER_IMPL_H
#define FLAX_WINDOWS_FIBER_IMPL_H

#include "Flax/Common.h"

namespace flax {

class WindowsFiberImpl {
public:
   WindowsFiberImpl(FiberAndMain fiberData, bool isMainFiber);

   ~WindowsFiberImpl();

   bool isValid() const {
      return address != nullptr;
   }

   static void swap(WindowsFiberImpl& from, WindowsFiberImpl& to);

private:
   void* address;
   bool mainFiber;
   FiberAndMain fiberAndMain;
};

} // namespace flax

#endif
