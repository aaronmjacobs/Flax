#include "Flax/WindowsFiberImpl.h"

#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace flax {

WindowsFiberImpl::WindowsFiberImpl(FiberAndMain fiberData, bool isMainFiber)
   : address(nullptr), mainFiber(isMainFiber), fiberAndMain(fiberData) {
   assert(fiberAndMain.fiber && fiberAndMain.mainFunction);

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
      assert(isValid());
      bool success = ConvertFiberToThread() != 0;
      assert(success);
   } else if (address) {
      DeleteFiber(address);
   }
}

// static
void WindowsFiberImpl::swap(WindowsFiberImpl& from, WindowsFiberImpl& to) {
   assert(from.isValid() && to.isValid() && GetCurrentFiber() == from.address && GetCurrentFiber() != to.address);
   SwitchToFiber(to.address);
}

} // namespace flax
