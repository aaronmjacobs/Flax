#ifndef FLAX_SCHEDULER_H
#define FLAX_SCHEDULER_H

#include "Flax/Common.h"

namespace flax {

/*!
 * Scheduler used to select which Fiber to yield to when Fiber::yield() is called.
 */
class Scheduler {
public:
   virtual ~Scheduler() = default;

   /*!
    * Selects the next Fiber that should be yielded to. If there is no valid Fiber to yield to, it should return nullptr.
    */
   virtual Fiber* next() = 0;

   /*!
    * Called when a new Fiber is created. When a scheduler is set as the Fiber scheduler, this function will be called for all existing Fibers.
    */
   virtual void onFiberCreated(Fiber* fiber) = 0;

   /*!
    * Called when a Fiber finishes execution (or is destroyed).
    */
   virtual void onFiberFinished(Fiber* fiber) = 0;

   /*!
    * Called when a Fiber is yielded to (either due to being selected by this scheduler, or by being explicitly yielded to by the user).
    */
   virtual void onFiberYieldedTo(Fiber* fiber) = 0;
};

} // namespace flax

#endif
