# Flax

## Introduction

Flax is a simple C++ library that allows you to create and manage fibers. It has both Windows and POSIX implementations.

## Usage

Code:
```c++
#include <Flax/Fiber.h>
#include <cstdio>

static void foo() {
   std::printf("foo() begin\n");
   flax::Fiber::yield();
   std::printf("foo() end\n");
}

static void bar() {
   std::printf("bar() begin\n");
   flax::Fiber::yield();
   std::printf("bar() end\n");
}

static void baz() {
   std::printf("baz() begin\n");
   flax::Fiber::yield();
   std::printf("baz() end\n");
}

int main(int argc, char* argv[]) {
   // Creating a fiber does not immediately begin its execution.
   std::unique_ptr<flax::Fiber> fooFiber = flax::Fiber::create(foo, "Foo Fiber");
   std::unique_ptr<flax::Fiber> barFiber = flax::Fiber::create(bar, "Bar Fiber");
   std::unique_ptr<flax::Fiber> bazFiber = flax::Fiber::create(baz, "Baz Fiber");
   
   // Calling Fiber::yield() uses a scheduler to select the next fiber.
   // By default, a round robin scheduler is used (though you can provide your own).
   std::printf("Before yield() calls\n");
   flax::Fiber::yield();
   std::printf("Between yield() calls\n");
   flax::Fiber::yield();
   std::printf("After yield() calls\n");
   
   // Fiber::create() takes a std::function, so you can pass lambdas that capture variables.
   int x = 0;
   std::unique_ptr<flax::Fiber> lambdaFiber = flax::Fiber::create([&x]() { ++x; }, "Lambda Fiber");
   
   // You can use Fiber::yieldTo() to explicitly yield to another Fiber.
   std::printf("Before yieldTo(): %d\n", x);
   flax::Fiber::yieldTo(*lambdaFiber);
   std::printf("After yieldTo(): %d\n", x);

   return 0;
}
```

Output:
```
Before yield() calls
foo() begin
bar() begin
baz() begin
Between yield() calls
foo() end
bar() end
baz() end
After yield() calls
Before yieldTo(): 0
After yieldTo(): 1
```

## Using a Custom Scheduler

In order to use a custom scheduler, first make sure you're building with scheduler support enabled (see info below). Next, write a class that inherits from `flax::Scheduler`. Finally, call `flax::Fiber::setScheduler()`. Documentation is available in [Include/Flax/Scheduler.h](Include/Flax/Scheduler.h).

## Compiling Flax

Flax is set up to be built with CMake.

To generate a static library, execute CMake with the root of the repo as the source directory. To compile Flax with scheduler support, enable the `FLAX_ENABLE_SCHEDULER` CMake option.

## Including Flax

Wherever you want to use Flax, just include the header:

```c++
#include <Flax/Fiber.h>
```

## Linking Against Flax

### Static

If Flax was built statically, just link against the generated static library.

### CMake

To compile Flax along with another application using CMake, first add the Flax subdirectory:

```cmake
add_subdirectory("path/to/Flax")
```

Next, specify the the location of the Flax includes:

```cmake
target_include_directories(<target> <INTERFACE|PUBLIC|PRIVATE> $<TARGET_PROPERTY:Flax,INTERFACE_INCLUDE_DIRECTORIES>)
```

Finally, link against the Flax library:

```cmake
target_link_libraries(<target> <INTERFACE|PUBLIC|PRIVATE> Flax $<TARGET_PROPERTY:Flax,INTERFACE_LINK_LIBRARIES>)
```
