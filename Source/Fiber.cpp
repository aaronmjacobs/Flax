#include "Flax/Fiber.h"
#include "Flax/RoundRobinScheduler.h"

#include <algorithm>
#include <cassert>

namespace flax {

// static
thread_local Fiber* Fiber::activeFiber = nullptr;

// static
thread_local std::vector<Fiber*> Fiber::fibers;

// static
thread_local Fiber::SchedulerContainer Fiber::scheduler(std::make_unique<RoundRobinScheduler>());

// static
Fiber& Fiber::getMainFiber() {
   static thread_local Fiber mainFiber(nullptr, "Main Fiber", true);
   assert(mainFiber.isValid() && !mainFiber.isFinished() && mainFiber.isMainFiber());

   return mainFiber;
}

// static
std::unique_ptr<Fiber> Fiber::create(const std::function<void()>& func, const std::string& name) {
   // Make sure the main fiber is initialized
   getMainFiber();

   std::unique_ptr<Fiber> newFiber(new Fiber(func, name, false));
   if (!newFiber->isValid()) {
      return nullptr;
   }

   return newFiber;
}

// static
void Fiber::yield() {
   Fiber* nextFiber = scheduler ? scheduler->next() : nullptr;
   if (!nextFiber) {
      return;
   }

   assert(nextFiber->isValid() && !nextFiber->isActive() && !nextFiber->isFinished());
   assert(nextFiber != activeFiber);

   yieldTo(*nextFiber);
}

// static
void Fiber::yieldTo(Fiber& fiber) {
   assert(fiber.isValid() && !fiber.isActive() && !fiber.isFinished());
   assert(activeFiber && activeFiber->isValid() && activeFiber->isActive());
   assert(&fiber != activeFiber);

   if (scheduler) {
      scheduler->onFiberYieldedTo(&fiber);
   }

   FiberImpl& lastFiberImpl = activeFiber->impl;
   activeFiber = &fiber;
   fiber.impl.swapTo(lastFiberImpl);
}

// static
void Fiber::setScheduler(std::unique_ptr<Scheduler> newScheduler) {
   scheduler = std::move(newScheduler);

   if (scheduler) {
      for (Fiber* fiber : fibers) {
         assert(fiber->isValid());
         scheduler->onFiberCreated(fiber);
      }
   }
}

// static
void Fiber::fiberMain(Fiber* fiber) {
   assert(fiber && fiber->isValid() && fiber->isActive() && !fiber->isFinished() && !fiber->isMainFiber() && fiber->function != nullptr);

   fiber->function();

   fiber->finish();
   fiber->yield();

   assert(false); // Should never get here
}

Fiber::Fiber(const std::function<void()>& func, const std::string& name, bool isMainFiber)
   : function(func), fiberName(name), mainFiber(isMainFiber), finished(false), impl(this, &Fiber::fiberMain, isMainFiber) {
   if (isMainFiber) {
      assert(isValid() && function == nullptr);
      activeFiber = this;
   }

   if (isValid()) {
      fibers.push_back(this);
      if (scheduler) {
         scheduler->onFiberCreated(this);
      }
   }
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

   if (scheduler) {
      scheduler->onFiberFinished(this);
   }

   fibers.erase(std::remove_if(fibers.begin(), fibers.end(), [this](const Fiber* fiber) { return fiber == this; }), fibers.end());
}

} // namespace flax
