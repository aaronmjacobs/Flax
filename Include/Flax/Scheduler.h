#ifndef FLAX_SCHEDULER_H
#define FLAX_SCHEDULER_H

#include "Flax/Common.h"

namespace flax {

class Scheduler {
public:
   virtual ~Scheduler() = default;

   virtual Fiber* next() = 0;

   virtual void onFiberCreated(Fiber* fiber) = 0;

   virtual void onFiberFinished(Fiber* fiber) = 0;

   virtual void onFiberYieldedTo(Fiber* fiber) = 0;
};

} // namespace flax

#endif
