#ifndef FLAX_COMMON_H
#define FLAX_COMMON_H

namespace flax {

class Fiber;

using FiberMainFunction = void(*)(Fiber*);

struct FiberAndMain {
   Fiber* fiber;
   FiberMainFunction mainFunction;

   FiberAndMain(Fiber* fib, FiberMainFunction mainFunc)
      : fiber(fib), mainFunction(mainFunc) {
   }
};

} // namespace flax

#endif
