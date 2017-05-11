#ifndef FLAX_FIBER_H
#define FLAX_FIBER_H

#include "Flax/FiberImpl.h"
#include "Flax/Scheduler.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace flax {

class Fiber {
public:
   static Fiber& getMainFiber();

   static std::unique_ptr<Fiber> create(const std::function<void()>& func, const std::string& name = "Fiber");

   static void yield();

   static void yieldTo(Fiber& fiber);

   static void setScheduler(std::unique_ptr<Scheduler> newScheduler);

   ~Fiber();

   const std::string& name() const {
      return fiberName;
   }

   bool isValid() const {
      return impl.isValid();
   }

   bool isActive() const {
      return activeFiber == this;
   }

   bool isFinished() const {
      return finished;
   }

   bool isMainFiber() const {
      return mainFiber;
   }

private:
   static void fiberMain(Fiber* fiber);

   Fiber(const std::function<void()>& func, const std::string& name, bool isMainFiber);
   Fiber(const Fiber& other) = delete;
   Fiber(Fiber&& other) = delete;
   Fiber& operator=(const Fiber& other) = delete;
   Fiber& operator=(Fiber&& other) = delete;

   void finish();

   class SchedulerContainer {
   public:
      SchedulerContainer()
         : scheduler(nullptr) {
      }

      SchedulerContainer(std::unique_ptr<Scheduler> other)
         : scheduler(std::move(other)) {
      }

      ~SchedulerContainer() {
         scheduler = nullptr;
      }

      SchedulerContainer& operator=(std::unique_ptr<Scheduler> other) {
         scheduler = std::move(other);
         return *this;
      }

      Scheduler* operator->() {
         return scheduler.get();
      }

      explicit operator bool() {
         return scheduler.operator bool();
      }

   private:
      std::unique_ptr<Scheduler> scheduler;
   };

   static thread_local SchedulerContainer scheduler;
   static thread_local Fiber* activeFiber;
   static thread_local std::vector<Fiber*> fibers;

   std::function<void()> function;
   std::string fiberName;
   const bool mainFiber;
   bool finished;
   FiberImpl impl;
};

} // namespace flax

#endif
