#ifndef FLAX_ROUND_ROBIN_SCHEDULER_H
#define FLAX_ROUND_ROBIN_SCHEDULER_H

#include "Flax/Scheduler.h"

#include <algorithm>
#include <cassert>
#include <deque>

namespace flax {

/*!
 * Round robin Fiber scheduler.
 */
class RoundRobinScheduler : public Scheduler {
public:
   RoundRobinScheduler()
      : scheduledFiber(nullptr) {
   }

   virtual ~RoundRobinScheduler() = default;

   virtual Fiber* next() override {
      if (fiberQueue.empty()) {
         return nullptr;
      }

      Fiber* lastScheduledFiber = scheduledFiber;
      scheduledFiber = fiberQueue.front();
      fiberQueue.pop_front();

      if (lastScheduledFiber) {
         fiberQueue.push_back(lastScheduledFiber);
      }

      return scheduledFiber;
   }

   virtual void onFiberCreated(Fiber* fiber) override {
      assert(fiber && scheduledFiber != fiber && std::find(fiberQueue.begin(), fiberQueue.end(), fiber) == fiberQueue.end());

      fiberQueue.push_back(fiber);
   }

   virtual void onFiberFinished(Fiber* fiber) override {
      assert(fiber && (fiber == scheduledFiber || std::find(fiberQueue.begin(), fiberQueue.end(), fiber) != fiberQueue.end()));

      if (scheduledFiber == fiber) {
         scheduledFiber = nullptr;
      }

      fiberQueue.erase(std::remove_if(fiberQueue.begin(), fiberQueue.end(), [fiber](Fiber* element) { return element == fiber; }), fiberQueue.end());
   }

   virtual void onFiberYieldedTo(Fiber* fiber) override {
      assert(fiber && (fiber == scheduledFiber || std::find(fiberQueue.begin(), fiberQueue.end(), fiber) != fiberQueue.end()));

      while (fiber != scheduledFiber) {
         next();
      }
   }

private:
   std::deque<Fiber*> fiberQueue;
   Fiber* scheduledFiber;
};

} // namespace flax

#endif
