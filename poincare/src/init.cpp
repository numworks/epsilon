#include <poincare/init.h>
#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/tree_pool.h>

namespace Poincare {

void Init() {
  // Create and register the shared static pool
  static TreePool pool
#if PLATFORM_DEVICE
    __attribute__((section(".bss.$poincare_pool")));
#else
    ;
#endif
  TreePool::RegisterPool(&pool);
}

}
