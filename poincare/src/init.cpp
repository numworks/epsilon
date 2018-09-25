#include <poincare/init.h>
#include <poincare/integer.h>
#include <poincare/tree_pool.h>

namespace Poincare {

void init() {
  // Create and register the shared static pool
  static TreePool pool;
  TreePool::RegisterPool(&pool);
}

void Tidy() {
  // Clean Integer
  Integer::TidyIntegerBuffer();
}

}
