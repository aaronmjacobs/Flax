#include "Flax/Fiber.h"
#include "Flax/RoundRobinScheduler.h"

#include <algorithm>
#include <cassert>

namespace flax {

#if FLAX_USE_SCHEDULER
// static
thread_local Fiber::SchedulerContainer Fiber::scheduler(std::make_unique<RoundRobinScheduler>());

// static
thread_local std::vector<Fiber*> Fiber::fibers;
#endif // FLAX_USE_SCHEDULER

// static
thread_local Fiber* Fiber::activeFiber = nullptr;

// static
Fiber& Fiber::getMainFiber() {
   static thread_local Fiber mainFiber(nullptr, "Main Fiber", true);
   assert(mainFiber.isValid() && !mainFiber.isFinished() && mainFiber.isMainFiber());

   return mainFiber;
}

// static
std::unique_ptr<Fiber> Fiber::create(const std::function<void()>& function, const std::string& name) {
   // Make sure the main fiber is initialized
   getMainFiber();

   std::unique_ptr<Fiber> newFiber(new Fiber(function, name, false));
   if (!newFiber->isValid()) {
      return nullptr;
   }

   return newFiber;
}

// static
void Fiber::yieldTo(Fiber& fiber) {
   assert(fiber.isValid() && !fiber.isActive() && !fiber.isFinished());
   assert(activeFiber && activeFiber->isValid() && activeFiber->isActive());
   assert(&fiber != activeFiber);

#if FLAX_USE_SCHEDULER
   scheduler->onFiberYieldedTo(&fiber);
#endif // FLAX_USE_SCHEDULER

   FiberImpl& from = activeFiber->impl;
   FiberImpl& to = fiber.impl;
   activeFiber = &fiber;
   FiberImpl::swap(from, to);
}

#if FLAX_USE_SCHEDULER
// static
void Fiber::yield() {
   Fiber* nextFiber = scheduler->next();
   if (!nextFiber) {
      return;
   }

   assert(nextFiber->isValid() && !nextFiber->isActive() && !nextFiber->isFinished());
   assert(nextFiber != activeFiber);

   yieldTo(*nextFiber);
}

// static
void Fiber::setScheduler(std::unique_ptr<Scheduler> newScheduler) {
   if (newScheduler) {
      scheduler = std::move(newScheduler);

      for (Fiber* fiber : fibers) {
         assert(fiber->isValid());
         scheduler->onFiberCreated(fiber);
      }

      if (activeFiber) {
         scheduler->onFiberYieldedTo(activeFiber);
      }
   }
}
#endif // FLAX_USE_SCHEDULER

// static
void Fiber::fiberMain(Fiber* fiber) {
   assert(fiber && fiber->isValid() && fiber->isActive() && !fiber->isFinished() && !fiber->isMainFiber() && fiber->function != nullptr);

   fiber->function();

   fiber->finish();
#if FLAX_USE_SCHEDULER
   fiber->yield();
#else
   fiber->yieldTo(getMainFiber());
#endif // FLAX_USE_SCHEDULER

   assert(false); // Should never get here
}

Fiber::Fiber(const std::function<void()>& func, const std::string& name, bool isMainFiber)
   : function(func), fiberName(name), mainFiber(isMainFiber), finished(false), impl(FiberAndMain(this, &Fiber::fiberMain), isMainFiber) {
   if (isMainFiber) {
      assert(isValid() && function == nullptr && activeFiber == nullptr);
      activeFiber = this;
   }

#if FLAX_USE_SCHEDULER
   if (isValid()) {
      fibers.push_back(this);
      scheduler->onFiberCreated(this);

      if (isMainFiber) {
         scheduler->onFiberYieldedTo(this);
      }
   }
#endif // FLAX_USE_SCHEDULER
}

Fiber::~Fiber() {
   assert(isActive() == isMainFiber());

   if (isValid() && !isFinished()) {
      finish();
   }
}

void Fiber::finish() {
   assert(isValid() && !isFinished());

   finished = true;

#if FLAX_USE_SCHEDULER
   if (scheduler) {
      scheduler->onFiberFinished(this);
   }
   fibers.erase(std::remove_if(fibers.begin(), fibers.end(), [this](const Fiber* fiber) { return fiber == this; }), fibers.end());
#endif // FLAX_USE_SCHEDULER
}

} // namespace flax
