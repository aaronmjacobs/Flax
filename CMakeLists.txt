# Project definition
cmake_minimum_required(VERSION 3.1)
project(Flax VERSION 1.0.0 LANGUAGES CXX)

set(LINUX FALSE)
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(LINUX TRUE)
endif()

# Options
option(FLAX_ENABLE_SCHEDULER "Enable support for using a scheduler to select fibers" OFF)
if (FLAX_ENABLE_SCHEDULER)
   set(FLAX_USE_SCHEDULER 1)
else ()
   set(FLAX_USE_SCHEDULER 0)
endif ()

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

set(SOURCES
   "${INC_DIR}/Flax/Common.h"
   "${INC_DIR}/Flax/Fiber.h"
   "${INC_DIR}/Flax/FiberImpl.h"
   "${SRC_DIR}/Fiber.cpp"
)

set(FLAX_PLATFORM_WIN 0)
set(FLAX_PLATFORM_MAC 0)
set(FLAX_PLATFORM_LINUX 0)
if (WIN32)
   set(FLAX_PLATFORM_WIN 1)
   list(APPEND SOURCES
      "${INC_DIR}/Flax/WindowsFiberImpl.h"
      "${SRC_DIR}/WindowsFiberImpl.cpp"
   )
elseif (APPLE)
   set(FLAX_PLATFORM_MAC 1)
   list(APPEND SOURCES
      "${INC_DIR}/Flax/PosixFiberImpl.h"
      "${SRC_DIR}/PosixFiberImpl.cpp"
   )
elseif (LINUX)
   set(FLAX_PLATFORM_LINUX 1)
   list(APPEND SOURCES
      "${INC_DIR}/Flax/PosixFiberImpl.h"
      "${SRC_DIR}/PosixFiberImpl.cpp"
   )
endif ()

if (FLAX_ENABLE_SCHEDULER)
   list(APPEND SOURCES
      "${INC_DIR}/Flax/Scheduler.h"
      "${INC_DIR}/Flax/RoundRobinScheduler.h"
   )
endif ()

# Library definition and features
add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${INC_DIR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC
   FLAX_PLATFORM_WIN=${FLAX_PLATFORM_WIN}
   FLAX_PLATFORM_MAC=${FLAX_PLATFORM_MAC}
   FLAX_PLATFORM_LINUX=${FLAX_PLATFORM_LINUX}
   FLAX_USE_SCHEDULER=${FLAX_USE_SCHEDULER}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)
