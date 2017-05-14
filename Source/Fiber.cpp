#include "Flax/Fiber.h"
#include "Flax/RoundRobinScheduler.h"

#include <algorithm>
#include <cassert>

namespace flax {

// static
thread_local std::shared_ptr<Fiber::ThreadLocalData> Fiber::threadLocalData;

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
   assert(threadLocalData && threadLocalData->activeFiber);
   assert(threadLocalData->activeFiber->isValid() && threadLocalData->activeFiber->isActive());
   assert(&fiber != threadLocalData->activeFiber);

#if FLAX_USE_SCHEDULER
   threadLocalData->scheduler->onFiberYieldedTo(&fiber);
#endif // FLAX_USE_SCHEDULER

   FiberImpl& from = threadLocalData->activeFiber->impl;
   FiberImpl& to = fiber.impl;
   threadLocalData->activeFiber = &fiber;
   FiberImpl::swap(from, to);
}

#if FLAX_USE_SCHEDULER
// static
void Fiber::yield() {
   assert(threadLocalData && threadLocalData->activeFiber && threadLocalData->scheduler);

   Fiber* nextFiber = threadLocalData->scheduler->next();
   if (!nextFiber) {
      return;
   }

   assert(nextFiber->isValid() && !nextFiber->isActive() && !nextFiber->isFinished());
   assert(nextFiber != threadLocalData->activeFiber);

   yieldTo(*nextFiber);
}

// static
void Fiber::setScheduler(std::unique_ptr<Scheduler> newScheduler) {
   assert(threadLocalData);

   if (newScheduler) {
      threadLocalData->scheduler = std::move(newScheduler);

      for (Fiber* fiber : threadLocalData->fibers) {
         assert(fiber->isValid());
         threadLocalData->scheduler->onFiberCreated(fiber);
      }

      if (threadLocalData->activeFiber) {
         threadLocalData->scheduler->onFiberYieldedTo(threadLocalData->activeFiber);
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
      assert(isValid() && function == nullptr && threadLocalData == nullptr);

      threadLocalData = std::shared_ptr<ThreadLocalData>(new ThreadLocalData{});
      threadLocalData->activeFiber = this;
#if FLAX_USE_SCHEDULER
      threadLocalData->scheduler = std::make_unique<RoundRobinScheduler>();
#endif // FLAX_USE_SCHEDULER
   }

   data = threadLocalData;
   assert(data);

#if FLAX_USE_SCHEDULER
   if (isValid()) {
      data->fibers.push_back(this);
      data->scheduler->onFiberCreated(this);

      if (isMainFiber) {
         data->scheduler->onFiberYieldedTo(this);
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
   data->scheduler->onFiberFinished(this);
   data->fibers.erase(std::remove_if(data->fibers.begin(), data->fibers.end(), [this](const Fiber* fiber) { return fiber == this; }), data->fibers.end());
#endif // FLAX_USE_SCHEDULER
}

} // namespace flax
