#ifndef FLAX_FIBER_H
#define FLAX_FIBER_H

#include "Flax/FiberImpl.h"

#if FLAX_USE_SCHEDULER
#include "Flax/Scheduler.h"
#include <vector>
#endif // FLAX_USE_SCHEDULER

#include <functional>
#include <memory>
#include <string>

namespace flax {

/*!
 * Class that represents a Fiber (lightweight thread of execution that allows for cooperative multitasking).
 * Fibers are bound to the thread they're created on.
 */
class Fiber {
public:

   /*!
    * Gets the (thread local) main Fiber singleton.
    */
   static Fiber& getMainFiber();

   /*!
    * Gets the (thread local) active Fiber. If no Fibers have been created (including the main fiber), nullptr will be returned.
    */
   static Fiber* getActiveFiber();

   /*!
    * Creates a new Fiber on the current thread that will run |function|. Returns a null pointer on failure.
    * When a created Fiber finishes executing, if a scheduler is being used, the next chosen Fiber will be yielded to.
    * If a scheduler is not being used, the Fiber will yield to the main Fiber.
    */
   static std::unique_ptr<Fiber> create(const std::function<void()>& function, const std::string& name = "Fiber");

   /*!
    * Yields execution to the given Fiber. You should not yield to the active Fiber or to a Fiber that has finished executing.
    */
   static void yieldTo(Fiber& fiber);

#if FLAX_USE_SCHEDULER
   /*!
    * Yields execution to the next Fiber chosen by the scheduler. If the scheduler can't select a Fiber, the current Fiber will continue execution.
    */
   static void yield();

   /*!
    * Sets the scheduler used to select Fibers when yield() is called. If no scheduler is set, a round robin scheduler is used by default.
    */
   static void setScheduler(std::unique_ptr<Scheduler> newScheduler);
#endif // FLAX_USE_SCHEDULER

   ~Fiber();

   /*!
    * Returns the name of the Fiber.
    */
   const std::string& name() const {
      return fiberName;
   }

   /*!
    * Returns whether the Fiber is the active Fiber.
    */
   bool isActive() const {
      return data->activeFiber == this;
   }

   /*!
    * Returns whether the Fiber has finished executing.
    */
   bool isFinished() const {
      return finished;
   }

   /*!
    * Returns whether the Fiber is the main Fiber.
    */
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

   bool isValid() const {
      return impl.isValid();
   }

   void finish();

   struct ThreadLocalData {
#if FLAX_USE_SCHEDULER
      ThreadLocalData(std::unique_ptr<Scheduler> initialScheduler)
         : activeFiber(nullptr), scheduler(std::move(initialScheduler)) {
      }
#else
      ThreadLocalData()
         : activeFiber(nullptr) {
      }
#endif // FLAX_USE_SCHEDULER

      Fiber* activeFiber;
#if FLAX_USE_SCHEDULER
      std::unique_ptr<Scheduler> scheduler;
      std::vector<Fiber*> fibers;
#endif // FLAX_USE_SCHEDULER
   };

   static thread_local std::shared_ptr<ThreadLocalData> threadLocalData;

   std::function<void()> function;
   std::string fiberName;
   const bool mainFiber;
   bool finished;
   FiberImpl impl;
   std::shared_ptr<ThreadLocalData> data;
};

} // namespace flax

#endif
