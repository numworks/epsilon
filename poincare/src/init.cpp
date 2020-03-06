#include <poincare/init.h>
#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/tree_pool.h>

namespace Poincare {

void Init() {
  // Create and register the shared static pool
  static TreePool pool;
  TreePool::RegisterPool(&pool);
}

void Tidy() {
  // Clean Expression (reset the SymbolReplacementsLock)
  Expression::Tidy();
}

}
