#include <poincare/init.h>
#include <poincare/old/pool.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#if POINCARE_TREE_STACK_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare {

void Init() {
  Preferences::Init();
  Pool::sharedPool.init();
  Internal::SharedTreeStack.init();
  Internal::Tree::Init();
}

void Shutdown() {
#if POINCARE_TREE_STACK_VISUALIZATION
  Internal::CloseLogger();
#endif
}

}  // namespace Poincare
