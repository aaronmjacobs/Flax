#ifndef FLAX_FIBER_IMPL_H
#define FLAX_FIBER_IMPL_H

#if FLAX_PLATFORM_WIN

#include "WindowsFiberImpl.h"
namespace flax {
using FiberImpl = WindowsFiberImpl;
} // namespace flax

#elif FLAX_PLATFORM_MAC || FLAX_PLATFORM_LINUX

#include "PosixFiberImpl.h"
namespace flax {
using FiberImpl = PosixFiberImpl;
} // namespace flax

#else

#error "Invalid platform"

#endif // FLAX_PLATFORM_<>

#endif
