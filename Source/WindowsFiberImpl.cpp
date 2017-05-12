#include "Flax/WindowsFiberImpl.h"

#include <cassert>

#include <Windows.h>

namespace flax {

WindowsFiberImpl::WindowsFiberImpl(Fiber* fiber, FiberMainFunction mainFunction, bool isMainFiber)
   : address(nullptr), mainFiber(isMainFiber), fiberAndMain(fiber, mainFunction) {
   assert(fiber && mainFunction);

   if (mainFiber) {
      address = ConvertThreadToFiber(this);
      assert(address);
   } else {
      address = CreateFiber(0, [](LPVOID param) {
         FiberAndMain* fiberAndMain = reinterpret_cast<FiberAndMain*>(param);
         assert(fiberAndMain);

         fiberAndMain->mainFunction(fiberAndMain->fiber);
      }, &fiberAndMain);
   }
}

WindowsFiberImpl::~WindowsFiberImpl() {
   if (mainFiber) {
      assert(address);
      bool success = ConvertFiberToThread() != 0;
      assert(success);
   } else if (address) {
      DeleteFiber(address);
   }

   address = nullptr;
}

// static
void WindowsFiberImpl::swap(WindowsFiberImpl& from, WindowsFiberImpl& to) {
   assert(from.address && to.address && GetCurrentFiber() == from.address && GetCurrentFiber() != to.address);
   SwitchToFiber(to.address);
}

} // namespace flax
