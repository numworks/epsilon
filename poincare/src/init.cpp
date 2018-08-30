#include <poincare/init.h>

namespace Poincare {

void init() {
  // Create and register the shared static pool
  static TreePool pool;
  TreePool::registerPool(&pool);
}

}
