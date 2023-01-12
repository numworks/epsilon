#ifndef UTILS_ASSERT_H
#define UTILS_ASSERT_H

#include <assert.h>

constexpr inline void constexpr_assert(bool check) {
#if ASSERTIONS
  void(0);
#else
  ((check) ? void(0) : []{assert(!#CHECK);}() )
#endif
}

#endif
